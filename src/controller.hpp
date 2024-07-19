/*
 * controller.hpp
 *
 * Copyright (C) 2023-2024 Max Qian <lightapt.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SODIUM_CONTROLLER_HPP
#define SODIUM_CONTROLLER_HPP

struct SettleParams {
    double tolerancePx;  // settle threshold, pixels
    int settleTimeSec;   // time to be within tolerance
    int timeoutSec;      // timeout value
    int frames;          // number of frames
};

enum ControlGuideOptions {
    GUIDEOPT_FORCE_RECAL = (1U << 0),  // Always recalibrate
    GUIDEOPT_USE_STICKY_LOCK =
        (1U
         << 1),  // Insure that recalibration returns star to initial position
};

class PhdController {
public:
    static bool CanGuide(wxString *error);
    static bool Guide(unsigned int options, const SettleParams &settle,
                      const wxRect &roi, wxString *error);
    static bool Dither(double pixels, bool forceRaOnly,
                       const SettleParams &settle, wxString *error);
    static bool Dither(double pixels, int settleFrames, wxString *error);
    static bool DitherCompat(double pixels, wxString *error);

    static void AbortController(const wxString &reason);
    static void UpdateControllerState();
    static void OnAppInit();
    static void OnAppExit();

    static bool IsSettling();
    static bool IsIdle();  // Not currently working through various transitions
                           // in guiding state
};

#endif
