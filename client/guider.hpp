/*

MIT License

Copyright (c) 2017 Andy Galasso

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef GUIDER_INCLUDED
#define GUIDER_INCLUDED

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// settling progress information returned by Guider::CheckSettling()
struct SettleProgress {
    bool Done;
    double Distance;
    double SettlePx;
    double Time;
    double SettleTime;
    int Status;
    std::string Error;
};

// guiding statistics information returned by Guider::GetStats()
struct GuideStats {
    double rms_tot;
    double rms_ra;
    double rms_dec;
    double peak_ra;
    double peak_dec;
};

// Guider - a C++ wrapper for the PHD2 server API
//    https://github.com/OpenPHDGuiding/phd2/wiki/EventMonitoring
//
class Guider {
    class Impl;
    std::unique_ptr<Impl> m_rep;

    Guider(const Guider&) = delete;
    Guider& operator=(const Guider&) = delete;

public:
    Guider(const char *hostname, unsigned int phd2_instance = 1);
    ~Guider();

    const std::string& LastError() const;

    bool Connect();
    void Disconnect();

    nlohmann::json Call(const std::string& method);
    nlohmann::json Call(const std::string& method, const nlohmann::json& params);

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

#endif // GUIDER_INCLUDED
