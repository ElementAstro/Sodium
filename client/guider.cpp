/*

MIT License

...

*/

#include "guider.hpp"

#include <atomic>
#include <condition_variable>
#include <curl/curl.h>
#include <deque>
#include <iostream>
#include <math.h>
#include <mutex>
#include <sstream>
#include <thread>
#include <poll.h>

#include <nlohmann/json.hpp>

#ifdef _MSC_VER
# include <Winsock2.h>
# define POLL WSAPoll
#else
# include <poll.h>
# define POLL poll
#endif

using json = nlohmann::json;

// class GuiderConnection
class GuiderConnection {
    CURL *m_curl;
    curl_socket_t m_sockfd;
    std::deque<std::string> m_dq;
    std::ostringstream m_os;
    std::atomic_bool m_terminate;
    
    bool WaitReadable();
    bool WaitWritable();

public:
    GuiderConnection();
    ~GuiderConnection();
    bool Connect(const char *hostname, unsigned short port);
    void Disconnect();
    bool IsConnected() const { return m_curl != nullptr; }
    std::string ReadLine();
    bool WriteLine(const std::string& s);
    void Terminate();
};

GuiderConnection::GuiderConnection() : m_curl(nullptr), m_terminate(false) {}

GuiderConnection::~GuiderConnection() {
    Disconnect();
}

bool GuiderConnection::Connect(const char *hostname, unsigned short port) {
    Disconnect();

    m_terminate = false;

    m_curl = curl_easy_init();
    if (!m_curl) return false;

    std::ostringstream os;
    os << "http://" << hostname << ':' << port;

    curl_easy_setopt(m_curl, CURLOPT_URL, os.str().c_str());
    curl_easy_setopt(m_curl, CURLOPT_CONNECT_ONLY, 1L);

    try {
        CURLcode res = curl_easy_perform(m_curl);
        if (res != CURLE_OK) throw res;

        res = curl_easy_getinfo(m_curl, CURLINFO_ACTIVESOCKET, &m_sockfd);
        if (res != CURLE_OK) throw res;

        return true;
    } catch (CURLcode /*res*/) {
        curl_easy_cleanup(m_curl);
        m_curl = nullptr;
        return false;
    }
}

void GuiderConnection::Disconnect() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
        m_curl = nullptr;
    }
}

bool GuiderConnection::WaitReadable() {
    struct pollfd pfd;
    pfd.fd = m_sockfd;
    pfd.events = POLLIN;

    while (!m_terminate) {
        int ret = POLL(&pfd, 1, 500);
        if (ret == 1) return true;
    }

    return false;
}

std::string GuiderConnection::ReadLine() {
    while (m_dq.empty()) {
        char buf[1024];
        size_t nbuf;

        while (true) {
            CURLcode res = curl_easy_recv(m_curl, buf, sizeof(buf), &nbuf);
            if (res == CURLE_OK) break;
            else if (res == CURLE_AGAIN) {
                if (!WaitReadable()) return "";
            } else {
                return "";
            }
        }

        const char *p0 = &buf[0];
        const char *p = p0;
        while (p < &buf[nbuf]) {
            if (*p == '\r' || *p == '\n') {
                m_os.write(p0, p - p0);
                if (m_os.tellp() > 0) {
                    m_dq.push_back(std::move(m_os.str()));
                    m_os.str("");
                }
                p0 = ++p;
            } else {
                ++p;
            }
        }
        m_os.write(p0, p - p0);
    }

    std::string sret = std::move(m_dq.front());
    m_dq.pop_front();

    return sret;
}

bool GuiderConnection::WaitWritable() {
    struct pollfd pfd;
    pfd.fd = m_sockfd;
    pfd.events = POLLOUT;

    int ret = POLL(&pfd, 1, -1);

    return ret > 0;
}

bool GuiderConnection::WriteLine(const std::string& s) {
    size_t rem = s.size();
    const char *pos = s.c_str();

    while (rem > 0) {
        size_t nwr;
        CURLcode res = curl_easy_send(m_curl, pos, rem, &nwr);
        if (res == CURLE_AGAIN) {
            WaitWritable();
            continue;
        }
        if (res != CURLE_OK) return false;
        pos += nwr;
        rem -= nwr;
    }

    return true;
}

void GuiderConnection::Terminate() {
    m_terminate = true;
}

// ==============================================

class Accum {
    unsigned int n;
    double a;
    double q;
    double peak;

public:
    Accum() { Reset(); }
    void Reset() { n = 0; a = q = peak = 0.; }
    void Add(double x) {
        double ax = fabs(x);
        if (ax > peak) peak = ax;
        ++n;
        double d = x - a;
        a += d / (double) n;
        q += (x - a) * d;
    }
    double Mean() const { return a; }
    double Stdev() const { return n >= 1 ? sqrt(q / (double) n) : 0.0; }
    double Peak() const { return peak; }
};

// =========================================================

static std::string Format(const char *fmt, ...) {
    char buf[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return buf;
}

// =========================================================

class Guider::Impl {
    std::string m_host;
    unsigned int m_instance;
    GuiderConnection m_conn;

    std::thread m_worker;

    std::atomic_bool m_terminate;

    std::mutex m_mutex;
    std::condition_variable m_cond;
    json m_response;

    Accum accum_ra;
    Accum accum_dec;
    bool accum_active;
    double settle_px;

    std::string AppState;
    double AvgDist;
    GuideStats Stats;
    std::string Version;
    std::string PHDSubver;
    std::unique_ptr<SettleProgress> mSettle;

    void Worker();
    void handle_event(const json& ev);

public:
    std::string Error;

    Impl(const char *hostname, unsigned int phd2_instance);
    ~Impl();

    bool Connect();
    void Disconnect();

    json Call(const std::string& method);
    json Call(const std::string& method, const json& params);

    bool Guide(double settlePixels, double settleTime, double settleTimeout);
    bool Dither(double ditherPixels, double settlePixels, double settleTime, double settleTimeout);
    bool IsSettling(bool *val);
    bool CheckSettling(SettleProgress *s);
    bool GetStats(GuideStats *stats);
    bool StopCapture(unsigned int timeoutSeconds = 10);
    bool Loop(unsigned int timeoutSeconds = 10);
    bool PixelScale(double *result);
    bool GetEquipmentProfiles(std::vector<std::string> *profiles);
    bool ConnectEquipment(const char *profileName);
    bool DisconnectEquipment();
    bool GetStatus(std::string *appState, double *avgDist);
    bool IsGuiding(bool *result);
    bool Pause();
    bool Unpause();
    bool SaveImage(std::string *filename);
};

Guider::Impl::Impl(const char *hostname, unsigned int phd2_instance)
    : m_host(hostname), m_instance(phd2_instance), m_terminate(false) {}

Guider::Impl::~Impl() {
    Disconnect();
}

bool Guider::Impl::Connect() {
    Disconnect();

    unsigned short port = 4400 + m_instance - 1;
    if (!m_conn.Connect(m_host.c_str(), port)) {
        Error = Format("Could not connect to PHD2 instance %d on %s", m_instance, m_host.c_str());
        return false;
    }

    m_terminate = false;
    m_worker = std::thread(&Impl::Worker, this);

    return true;
}

void Guider::Impl::Disconnect() {
    if (m_worker.joinable()) {
        m_terminate = true;
        m_conn.Terminate();
        m_worker.join();
    }

    m_conn.Disconnect();
}

static void accum_get_stats(GuideStats *stats, const Accum& ra, const Accum& dec) {
    stats->rms_ra = ra.Stdev();
    stats->rms_dec = dec.Stdev();
    stats->peak_ra = ra.Peak();
    stats->peak_dec = dec.Peak();
}

static bool is_guiding(const std::string& st) {
    return st == "Guiding" || st == "LostLock";
}

void Guider::Impl::handle_event(const json& ev) {
    const std::string e = ev["Event"].get<std::string>();

    if (e == "AppState") {
        std::unique_lock<std::mutex> _lock(m_mutex);
        AppState = ev["State"].get<std::string>();
        if (is_guiding(AppState))
            AvgDist = 0.;  // until we get a GuideStep event
    } else if (e == "Version") {
        std::unique_lock<std::mutex> _lock(m_mutex);
        Version = ev["PHDVersion"].get<std::string>();
        PHDSubver = ev["PHDSubver"].get<std::string>();
    } else if (e == "StartGuiding") {
        accum_active = true;
        accum_ra.Reset();
        accum_dec.Reset();

        GuideStats stats;
        accum_get_stats(&stats, accum_ra, accum_dec);

        {
            std::unique_lock<std::mutex> _lock(m_mutex);
            Stats = stats;
        }
    } else if (e == "GuideStep") {
        GuideStats stats;
        if (accum_active) {
            accum_ra.Add(ev["RADistanceRaw"].get<double>());
            accum_dec.Add(ev["DECDistanceRaw"].get<double>());
            accum_get_stats(&stats, accum_ra, accum_dec);
        }
        std::unique_lock<std::mutex> _lock(m_mutex);
        AppState = "Guiding";
        AvgDist = ev["AvgDist"].get<double>();
        if (accum_active)
            Stats = stats;
    } else if (e == "SettleBegin") {
        accum_active = false;  // exclude GuideStep messages from stats while settling
    } else if (e == "Settling") {
        auto s = std::make_unique<SettleProgress>();
        s->Done = false;
        s->Distance = ev["Distance"].get<double>();
        s->SettlePx = settle_px;
        s->Time = ev["Time"].get<double>();
        s->SettleTime = ev["SettleTime"].get<double>();
        s->Status = 0;
        {
            std::unique_lock<std::mutex> _lock(m_mutex);
            mSettle.swap(s);
        }
    } else if (e == "SettleDone") {
        accum_active = true;
        accum_ra.Reset();
        accum_dec.Reset();

        GuideStats stats;
        accum_get_stats(&stats, accum_ra, accum_dec);

        auto s = std::make_unique<SettleProgress>();
        s->Done = true;
        s->Status = ev["Status"].get<int>();
        s->Error = ev["Error"].get<std::string>();

        {
            std::unique_lock<std::mutex> _lock(m_mutex);
            mSettle.swap(s);
            Stats = stats;
        }
    } else if (e == "Paused") {
        std::unique_lock<std::mutex> _lock(m_mutex);
        AppState = "Paused";
    } else if (e == "StartCalibration") {
        std::unique_lock<std::mutex> _lock(m_mutex);
        AppState = "Calibrating";
    } else if (e == "LoopingExposures") {
        std::unique_lock<std::mutex> _lock(m_mutex);
        AppState = "Looping";
    } else if (e == "LoopingExposuresStopped" || e == "GuidingStopped") {
        std::unique_lock<std::mutex> _lock(m_mutex);
        AppState = "Stopped";
    } else if (e == "StarLost") {
        std::unique_lock<std::mutex> _lock(m_mutex);
        AppState = "LostLock";
        AvgDist = ev["AvgDist"].get<double>();
    }
}

void Guider::Impl::Worker() {
    while (!m_terminate) {
        std::string line = m_conn.ReadLine();
        if (line.empty()) break;

        std::istringstream is(line);
        json j;
        try {
            is >> j;
        } catch (const std::exception& ex) {
            continue;
        }

        if (j.contains("jsonrpc")) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_response = j;
            m_cond.notify_one();
        } else {
            handle_event(j);
        }
    }
}

static std::string make_jsonrpc(const std::string& method, const json& params) {
    json req;
    req["method"] = method;
    req["id"] = 1;

    if (!params.is_null()) {
        if (params.is_array() || params.is_object())
            req["params"] = params;
        else {
            json ary = json::array();
            ary.push_back(params);
            req["params"] = ary;
        }
    }

    return req.dump();
}

static bool failed(const json& res) {
    return res.contains("error");
}

json Guider::Impl::Call(const std::string& method, const json& params) {
    std::string s = make_jsonrpc(method, params);
    m_conn.WriteLine(s);

    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_response.is_null())
        m_cond.wait(lock);

    json response;
    std::swap(m_response, response);

    if (failed(response))
        Error = response["error"]["message"].get<std::string>();

    return response;
}

json Guider::Impl::Call(const std::string& method) {
    return Call(method, json());
}

static json SettleParam(double settlePixels, double settleTime, double settleTimeout) {
    json s;
    s["pixels"] = settlePixels;
    s["time"] = settleTime;
    s["timeout"] = settleTimeout;
    return s;
}

bool Guider::Impl::Guide(double settlePixels, double settleTime, double settleTimeout) {
    auto s = std::make_unique<SettleProgress>();
    s->Done = false;
    s->Distance = 0.;
    s->SettlePx = settlePixels;
    s->Time = 0.;
    s->SettleTime = settleTime;
    s->Status = 0;

    {
        std::unique_lock<std::mutex> _lock(m_mutex);
        if (mSettle && !mSettle->Done) {
            Error = "cannot guide while settling";
            return false;
        }
        mSettle.swap(s);
    }

    json params = json::array({SettleParam(settlePixels, settleTime, settleTimeout), false});

    json res = Call("guide", params);

    if (!failed(res)) {
        settle_px = settlePixels;
        return true;
    }

    {
        std::unique_lock<std::mutex> _lock(m_mutex);
        mSettle.reset();
    }

    return false;
}

bool Guider::Impl::Dither(double ditherPixels, double settlePixels, double settleTime, double settleTimeout) {
    auto s = std::make_unique<SettleProgress>();
    s->Done = false;
    s->Distance = ditherPixels;
    s->SettlePx = settlePixels;
    s->Time = 0.;
    s->SettleTime = settleTime;
    s->Status = 0;

    {
        std::unique_lock<std::mutex> _lock(m_mutex);
        if (mSettle && !mSettle->Done) {
            Error = "cannot dither while settling";
            return false;
        }
        mSettle.swap(s);
    }

    json params = json::array({ditherPixels, false, SettleParam(settlePixels, settleTime, settleTimeout)});

    json ret = Call("dither", params);

    if (!failed(ret)) {
        settle_px = settlePixels;
        return true;
    }

    {
        std::unique_lock<std::mutex> _lock(m_mutex);
        mSettle.reset();
    }

    return false;
}

bool Guider::Impl::IsSettling(bool *ret) {
    {
        std::unique_lock<std::mutex> _lock(m_mutex);
        if (mSettle) {
            *ret = true;
            return true;
        }
    }

    json res = Call("get_settling");
    if (failed(res)) return false;

    bool val = res["result"].get<bool>();

    if (val) {
        auto s = std::make_unique<SettleProgress>();
        s->Done = false;
        s->Distance = -1.;
        s->SettlePx = 0.;
        s->Time = 0.;
        s->SettleTime = 0.;
        s->Status = 0;

        std::unique_lock<std::mutex> _lock(m_mutex);
        if (!mSettle) mSettle.swap(s);
    }

    *ret = val;
    return true;
}

bool Guider::Impl::CheckSettling(SettleProgress *s) {
    std::unique_ptr<SettleProgress> tmp;
    std::unique_lock<std::mutex> _lock(m_mutex);

    if (!mSettle) {
        Error = "not settling";
        return false;
    }

    if (mSettle->Done) {
        mSettle.swap(tmp);
        _lock.unlock();
        s->Done = true;
        s->Status = tmp->Status;
        s->Error = std::move(tmp->Error);
        return true;
    }

    s->Done = false;
    s->Distance = mSettle->Distance;
    s->SettlePx = settle_px;
    s->Time = mSettle->Time;
    s->SettleTime = mSettle->SettleTime;

    return true;
}

bool Guider::Impl::GetStats(GuideStats *stats) {
    std::unique_lock<std::mutex> _lock(m_mutex);
    *stats = Stats;
    stats->rms_tot = hypot(stats->rms_ra, stats->rms_dec);
    return true;
}

bool Guider::Impl::StopCapture(unsigned int timeoutSeconds) {
    json res = Call("stop_capture");
    if (failed(res)) return false;

    for (unsigned int i = 0; i < timeoutSeconds; i++) {
        std::string appstate;
        {
            std::unique_lock<std::mutex> _lock(m_mutex);
            appstate = AppState;
        }
        if (appstate == "Stopped") return true;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    res = Call("get_app_state");
    if (failed(res)) return false;

    std::string st = res["result"].get<std::string>();

    {
        std::unique_lock<std::mutex> _lock(m_mutex);
        AppState = st;
    }

    if (st == "Stopped") return true;

    Error = Format("guider did not stop capture after %d seconds!", timeoutSeconds);
    return false;
}

bool Guider::Impl::Loop(unsigned int timeoutSeconds) {
    {
        std::unique_lock<std::mutex> _lock(m_mutex);
        if (AppState == "Looping") return true;
    }

    json res = Call("get_exposure");
    if (failed(res)) return false;

    int exp = res["result"].get<int>();

    res = Call("loop");
    if (failed(res)) return false;

    std::this_thread::sleep_for(std::chrono::milliseconds(exp));

    for (unsigned int i = 0; i < timeoutSeconds; i++) {
        {
            std::unique_lock<std::mutex> _lock(m_mutex);
            if (AppState == "Looping") return true;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Error = "timed-out waiting for guiding to start looping";
    return false;
}

bool Guider::Impl::PixelScale(double *result) {
    json res = Call("get_pixel_scale");
    if (failed(res)) return false;

    *result = res["result"].get<double>();
    return true;
}

bool Guider::Impl::GetEquipmentProfiles(std::vector<std::string> *profiles) {
    json res = Call("get_profiles");
    if (failed(res)) return false;

    profiles->clear();

    json ary = res["result"];
    for (const auto& p : ary) {
        profiles->push_back(p["name"].get<std::string>());
    }

    return true;
}

bool Guider::Impl::ConnectEquipment(const char *profileName) {
    json res = Call("get_profile");
    if (failed(res)) return false;
    json prof = res["result"];

    std::string profname(profileName);

    if (prof["name"].get<std::string>() != profname) {
        res = Call("get_profiles");
        if (failed(res)) return false;

        json profiles = res["result"];
        int profid = -1;
        for (const auto& p : profiles) {
            if (p["name"].get<std::string>() == profname) {
                profid = p["id"].get<int>();
                break;
            }
        }
        if (profid == -1) {
            Error = "invalid phd2 profile name: " + profname;
            return false;
        }

        if (!StopCapture()) return false;
        res = Call("set_connected", false);
        if (failed(res)) return false;

        res = Call("set_profile", profid);
        if (failed(res)) return false;
    }

    res = Call("set_connected", true);
    return !failed(res);
}

bool Guider::Impl::DisconnectEquipment() {
    if (!StopCapture()) return false;

    json res = Call("set_connected", false);
    return !failed(res);
}

bool Guider::Impl::GetStatus(std::string *appState, double *avgDist) {
    std::unique_lock<std::mutex> _lock(m_mutex);
    *appState = AppState;
    *avgDist = AvgDist;
    return true;
}

bool Guider::Impl::IsGuiding(bool *result) {
    std::string st;
    double dist;
    if (!GetStatus(&st, &dist)) return false;
    *result = is_guiding(st);
    return true;
}

bool Guider::Impl::Pause() {
    return !failed(Call("set_paused", true));
}

bool Guider::Impl::Unpause() {
    return !failed(Call("set_paused", false));
}

bool Guider::Impl::SaveImage(std::string *filename) {
    json res = Call("save_image");
    if (failed(res)) return false;

    *filename = res["result"]["filename"].get<std::string>();
    return true;
}

// ======================================================================

Guider::Guider(const char *hostname, unsigned int phd2_instance)
    : m_rep(std::make_unique<Impl>(hostname, phd2_instance)) {}

Guider::~Guider() = default;

const std::string& Guider::LastError() const {
    return m_rep->Error;
}

bool Guider::Connect() {
    return m_rep->Connect();
}

void Guider::Disconnect() {
    m_rep->Disconnect();
}

json Guider::Call(const std::string& method) {
    return m_rep->Call(method);
}

json Guider::Call(const std::string& method, const json& params) {
    return m_rep->Call(method, params);
}

bool Guider::Guide(double settlePixels, double settleTime, double settleTimeout) {
    return m_rep->Guide(settlePixels, settleTime, settleTimeout);
}

bool Guider::Dither(double ditherPixels, double settlePixels, double settleTime, double settleTimeout) {
    return m_rep->Dither(ditherPixels, settlePixels, settleTime, settleTimeout);
}

bool Guider::IsSettling(bool *val) {
    return m_rep->IsSettling(val);
}

bool Guider::CheckSettling(SettleProgress *s) {
    return m_rep->CheckSettling(s);
}

bool Guider::GetStats(GuideStats *stats) {
    return m_rep->GetStats(stats);
}

bool Guider::StopCapture(unsigned int timeoutSeconds) {
    return m_rep->StopCapture(timeoutSeconds);
}

bool Guider::Loop(unsigned int timeoutSeconds) {
    return m_rep->Loop(timeoutSeconds);
}

bool Guider::PixelScale(double *result)
{
    return m_rep->PixelScale(result);
}

bool Guider::GetEquipmentProfiles(std::vector<std::string> *profiles) {
    return m_rep->GetEquipmentProfiles(profiles);
}

bool Guider::ConnectEquipment(const char *profileName) {
    return m_rep->ConnectEquipment(profileName);
}

bool Guider::DisconnectEquipment() {
    return m_rep->DisconnectEquipment();
}

bool Guider::GetStatus(std::string *appState, double *avgDist) {
    return m_rep->GetStatus(appState, avgDist);
}

bool Guider::IsGuiding(bool *result) {
    return m_rep->IsGuiding(result);
}

bool Guider::Pause() {
    return m_rep->Pause();
}

bool Guider::Unpause() {
    return m_rep->Unpause();
}

bool Guider::SaveImage(std::string *filename) {
    return m_rep->SaveImage(filename);
}
