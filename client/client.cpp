#include <asio.hpp>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>

using json = nlohmann::json;
using asio::ip::tcp;

class SettleProgress {
public:
    bool Done = false;
    double Distance = 0.0;
    double SettlePx = 0.0;
    double Time = 0.0;
    double SettleTime = 0.0;
    int Status = 0;
    std::string Error;

    SettleProgress() = default;
};

class GuideStats {
public:
    double rms_tot = 0.0;
    double rms_ra = 0.0;
    double rms_dec = 0.0;
    double peak_ra = 0.0;
    double peak_dec = 0.0;

    GuideStats() = default;
};

class GuiderClientException : public std::exception {
public:
    explicit GuiderClientException(const std::string& message) : message(message) {}
    const char* what() const noexcept override { return message.c_str(); }

private:
    std::string message;
};

class Accum {
public:
    Accum() { Reset(); }
    void Reset() {
        n = 0;
        a = q = peak = 0;
    }
    void Add(double x) {
        double ax = std::abs(x);
        if (ax > peak) {
            peak = ax;
        }
        n++;
        double d = x - a;
        a += d / n;
        q += (x - a) * d;
    }
    double Mean() const { return a; }
    double Stdev() const { return std::sqrt(q / n); }
    double Peak() const { return peak; }

private:
    int n = 0;
    double a = 0.0, q = 0.0, peak = 0.0;
};

class Conn {
public:
    Conn()
        : terminate(false),
          io_context(),
          resolver(io_context),
          sock(io_context) {}

    ~Conn() { Disconnect(); }

    void Connect(const std::string& hostname, int port) {
        try {
            asio::connect(sock,
                          resolver.resolve(hostname, std::to_string(port)));
            sock.non_blocking(true);
        } catch (std::exception& e) {
            throw GuiderClientException("Failed to connect: " +
                                  std::string(e.what()));
        }
    }

    void Disconnect() { sock.close(); }

    bool IsConnected() const { return sock.is_open(); }

    std::string ReadLine() {
        std::unique_lock lock(mutex);
        while (lines.empty() && !terminate) {
            cond.wait(lock);
        }
        if (terminate)
            return "";
        std::string line = lines.front();
        lines.erase(lines.begin());
        return line;
    }

    void WriteLine(const std::string& s) { asio::write(sock, asio::buffer(s)); }

    void Terminate() {
        terminate = true;
        cond.notify_all();
    }

    void StartReading() {
        read_thread = std::jthread([this]() { ReadFromSocket(); });
    }

private:
    void ReadFromSocket() {
        asio::streambuf buffer;
        while (!terminate) {
            try {
                asio::read_until(sock, buffer, "\r\n");
                std::istream is(&buffer);
                std::string line;
                std::getline(is, line);
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                {
                    std::lock_guard lock(mutex);
                    lines.push_back(line);
                }
                cond.notify_all();
            } catch (std::exception& e) {
                if (terminate)
                    break;
            }
        }
    }

    asio::io_context io_context;
    tcp::resolver resolver;
    tcp::socket sock;
    std::vector<std::string> lines;
    std::mutex mutex;
    std::condition_variable cond;
    std::jthread read_thread;
    bool terminate;
};

class GuiderClient {
public:
    static constexpr int DEFAULT_STOPCAPTURE_TIMEOUT = 10;

    GuiderClient(const std::string& hostname = "localhost", int instance = 1)
        : hostname(hostname),
          instance(instance),
          terminate(false),
          AppState(""),
          AvgDist(0),
          Version(""),
          PHDSubver(""),
          accum_active(false),
          settle_px(0) {
        Stats = std::make_shared<GuideStats>();
        Settle = std::make_shared<SettleProgress>();
    }

    ~GuiderClient() { Disconnect(); }

    void Connect() {
        Disconnect();
        try {
            conn = std::make_shared<Conn>();
            conn->Connect(hostname, 4400 + instance - 1);
            conn->StartReading();
            terminate = false;
            worker = std::jthread([this]() { Worker(); });
        } catch (std::exception& e) {
            Disconnect();
            throw;
        }
    }

    void Disconnect() {
        if (worker.joinable()) {
            terminate = true;
            conn->Terminate();
            worker.join();
        }
        if (conn) {
            conn->Disconnect();
            conn.reset();
        }
    }

    void Guide(double settlePixels, double settleTime, double settleTimeout) {
        CheckConnected();
        auto s = std::make_shared<SettleProgress>();
        s->Done = false;
        s->Distance = 0;
        s->SettlePx = settlePixels;
        s->Time = 0;
        s->SettleTime = settleTime;
        s->Status = 0;
        {
            std::lock_guard lock(mutex);
            if (Settle && !Settle->Done) {
                throw GuiderClientException("Cannot guide while settling");
            }
            Settle = s;
        }
        try {
            Call("guide", json::array({{"pixels", settlePixels},
                                       {"time", settleTime},
                                       {"timeout", settleTimeout},
                                       false}));
            settle_px = settlePixels;
        } catch (...) {
            std::lock_guard lock(mutex);
            Settle.reset();
            throw;
        }
    }

    void Dither(double ditherPixels, double settlePixels, double settleTime,
                double settleTimeout) {
        CheckConnected();
        auto s = std::make_shared<SettleProgress>();
        s->Done = false;
        s->Distance = ditherPixels;
        s->SettlePx = settlePixels;
        s->Time = 0;
        s->SettleTime = settleTime;
        s->Status = 0;
        {
            std::lock_guard lock(mutex);
            if (Settle && !Settle->Done) {
                throw GuiderClientException("Cannot dither while settling");
            }
            Settle = s;
        }
        try {
            Call("dither", json::array({ditherPixels,
                                        false,
                                        {{"pixels", settlePixels},
                                         {"time", settleTime},
                                         {"timeout", settleTimeout}}}));
            settle_px = settlePixels;
        } catch (...) {
            std::lock_guard lock(mutex);
            Settle.reset();
            throw;
        }
    }

    bool IsSettling() {
        CheckConnected();
        std::lock_guard lock(mutex);
        return Settle && !Settle->Done;
    }

    SettleProgress CheckSettling() {
        CheckConnected();
        std::lock_guard lock(mutex);
        if (!Settle) {
            throw GuiderClientException("Not settling");
        }
        return *Settle;
    }

    GuideStats GetStats() {
        CheckConnected();
        std::lock_guard lock(mutex);
        GuideStats stats = *Stats;
        stats.rms_tot = std::hypot(stats.rms_ra, stats.rms_dec);
        return stats;
    }

    void StopCapture(int timeoutSeconds = 10) {
        Call("stop_capture");
        WaitForState("Stopped", timeoutSeconds);
    }

    void Loop(int timeoutSeconds = 10) {
        CheckConnected();
        std::lock_guard lock(mutex);
        if (AppState == "Looping")
            return;

        auto res = Call("get_exposure");
        int exp_ms = res["result"];
        Call("loop");
        std::this_thread::sleep_for(std::chrono::milliseconds(exp_ms));

        WaitForState("Looping", timeoutSeconds);
    }

    double PixelScale() {
        auto res = Call("get_pixel_scale");
        return res["result"];
    }

    std::vector<std::string> GetEquipmentProfiles() {
        auto res = Call("get_profiles");
        std::vector<std::string> profiles;
        for (const auto& p : res["result"]) {
            profiles.push_back(p["name"]);
        }
        return profiles;
    }

    void ConnectEquipment(const std::string& profileName) {
        auto res = Call("get_profile");
        auto prof = res["result"];
        if (prof["name"] != profileName) {
            res = Call("get_profiles");
            int profid = -1;
            for (const auto& p : res["result"]) {
                if (p["name"] == profileName) {
                    profid = p["id"];
                    break;
                }
            }
            if (profid == -1) {
                throw GuiderClientException("Invalid PHD2 profile name: " +
                                      profileName);
            }
            StopCapture(DEFAULT_STOPCAPTURE_TIMEOUT);
            Call("set_connected", false);
            Call("set_profile", profid);
        }
        Call("set_connected", true);
    }

    void DisconnectEquipment() {
        StopCapture(DEFAULT_STOPCAPTURE_TIMEOUT);
        Call("set_connected", false);
    }

    std::pair<std::string, double> GetStatus() {
        CheckConnected();
        std::lock_guard lock(mutex);
        return {AppState, AvgDist};
    }

    bool IsGuiding() {
        auto [state, _] = GetStatus();
        return state == "Guiding" || state == "LostLock";
    }

    void Pause() { Call("set_paused", true); }

    void Unpause() { Call("set_paused", false); }

    std::string SaveImage() {
        auto res = Call("save_image");
        return res["result"]["filename"];
    }

private:
    std::string hostname;
    int instance;
    bool terminate;
    std::shared_ptr<Conn> conn;
    std::jthread worker;
    std::mutex mutex;
    std::condition_variable cond;
    json response;

    std::string AppState;
    double AvgDist;
    std::string Version;
    std::string PHDSubver;
    bool accum_active;
    double settle_px;
    Accum accum_ra;
    Accum accum_dec;
    std::shared_ptr<GuideStats> Stats;
    std::shared_ptr<SettleProgress> Settle;

    static bool IsGuidingState(const std::string& state) {
        return state == "Guiding" || state == "LostLock";
    }

    void Worker() {
        while (!terminate) {
            std::string line = conn->ReadLine();
            if (line.empty())
                break;

            try {
                auto j = json::parse(line);
                if (j.contains("jsonrpc")) {
                    std::lock_guard lock(mutex);
                    response = j;
                    cond.notify_all();
                } else {
                    HandleEvent(j);
                }
            } catch (json::parse_error&) {
                // Ignore invalid JSON
            }
        }
    }

    void HandleEvent(const json& ev) {
        std::string e = ev.value("Event", "");
        std::lock_guard lock(mutex);
        if (e == "AppState") {
            AppState = ev["State"];
            if (IsGuidingState(AppState)) {
                AvgDist = 0;
            }
        } else if (e == "Version") {
            Version = ev["SodiumVersion"];
            PHDSubver = ev["SodiumSubver"];
        } else if (e == "StartGuiding") {
            accum_active = true;
            accum_ra.Reset();
            accum_dec.Reset();
            Stats = std::make_shared<GuideStats>();
        } else if (e == "GuideStep") {
            if (accum_active) {
                accum_ra.Add(ev["RADistanceRaw"]);
                accum_dec.Add(ev["DECDistanceRaw"]);
                Stats = std::make_shared<GuideStats>(
                    AccumGetStats(accum_ra, accum_dec));
            }
            AppState = "Guiding";
            AvgDist = ev["AvgDist"];
        } else if (e == "SettleBegin") {
            accum_active = false;
        } else if (e == "Settling") {
            auto s = std::make_shared<SettleProgress>();
            s->Done = false;
            s->Distance = ev["Distance"];
            s->SettlePx = settle_px;
            s->Time = ev["Time"];
            s->SettleTime = ev["SettleTime"];
            Settle = s;
        } else if (e == "SettleDone") {
            accum_active = true;
            accum_ra.Reset();
            accum_dec.Reset();
            Stats = std::make_shared<GuideStats>(
                AccumGetStats(accum_ra, accum_dec));
            auto s = std::make_shared<SettleProgress>();
            s->Done = true;
            s->Status = ev["Status"];
            s->Error = ev.value("Error", "");
            Settle = s;
        } else if (e == "Paused") {
            AppState = "Paused";
        } else if (e == "StartCalibration") {
            AppState = "Calibrating";
        } else if (e == "LoopingExposures") {
            AppState = "Looping";
        } else if (e == "LoopingExposuresStopped" || e == "GuidingStopped") {
            AppState = "Stopped";
        } else if (e == "StarLost") {
            AppState = "LostLock";
            AvgDist = ev["AvgDist"];
        }
    }

    GuideStats AccumGetStats(const Accum& ra, const Accum& dec) const {
        GuideStats stats;
        stats.rms_ra = ra.Stdev();
        stats.rms_dec = dec.Stdev();
        stats.peak_ra = ra.Peak();
        stats.peak_dec = dec.Peak();
        return stats;
    }

    json Call(const std::string& method, const json& params = nullptr) {
        json req = {{"jsonrpc", "2.0"}, {"method", method}, {"id", 1}};
        if (params) {
            req["params"] = params;
        }
        conn->WriteLine(req.dump() + "\r\n");
        std::unique_lock lock(mutex);
        cond.wait(lock, [this] { return !response.is_null(); });
        json res = response;
        response = nullptr;
        if (res.contains("error")) {
            throw GuiderClientException(res["error"]["message"]);
        }
        return res;
    }

    void CheckConnected() {
        if (!conn->IsConnected()) {
            throw GuiderClientException("PHD2 Server disconnected");
        }
    }

    void WaitForState(const std::string& expectedState, int timeoutSeconds) {
        for (int i = 0; i < timeoutSeconds; ++i) {
            {
                std::lock_guard lock(mutex);
                if (AppState == expectedState)
                    return;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            CheckConnected();
        }
        auto res = Call("get_app_state");
        std::lock_guard lock(mutex);
        AppState = res["result"];
        if (AppState != expectedState) {
            throw GuiderClientException("Timed out waiting for state: " +
                                  expectedState);
        }
    }
};

int main() {
    try {
        GuiderClient guider;
        guider.Connect();
        for (const auto& profile : guider.GetEquipmentProfiles()) {
            std::cout << "Profile: " << profile << std::endl;
        }
        guider.Guide(1.5, 10.0, 60.0);
        // Add additional operations as needed
        guider.Disconnect();
    } catch (const GuiderClientException& ex) {
        std::cerr << "GuiderClientException: " << ex.what() << std::endl;
    }
    return 0;
}
