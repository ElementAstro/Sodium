/*
 * time.hpp
 * 
 * Copyright (C) 2023 Max Qian <lightapt.com>
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

/************************************************* 
 
Copyright: 2023 Max Qian. All rights reserved
 
Author: Max Qian

E-mail: astro_air@126.com
 
Date: 2023-3-20
 
Description: File Monitor
 
**************************************************/

#include <iostream>
#include <unistd.h>

#ifdef __WIN32__
    #include <windows.h>
#else
    #include <ctime>
#endif

class SystemTime {
    public:
        SystemTime() {
            #ifdef _WIN32
                SYSTEMTIME sysTime;
                GetSystemTime(&sysTime);
                m_year = sysTime.wYear;
                m_month = sysTime.wMonth;
                m_day = sysTime.wDay;
                m_hour = sysTime.wHour;
                m_minute = sysTime.wMinute;
                m_second = sysTime.wSecond;
            #else
                time_t t = std::time(nullptr);
                tm local_time = *std::localtime(&t);
                m_year = local_time.tm_year + 1900;
                m_month = local_time.tm_mon + 1;
                m_day = local_time.tm_mday;
                m_hour = local_time.tm_hour;
                m_minute = local_time.tm_min;
                m_second = local_time.tm_sec;
            #endif
        }

        void setTime(int year, int month, int day, int hour, int minute, int second) {
            if (geteuid() != 0) {
                std::cerr << "Permission denied. Need root privilege to set system time.\n";
                return;
            }

            #ifdef _WIN32
                SYSTEMTIME sysTime;
                sysTime.wYear = year;
                sysTime.wMonth = month;
                sysTime.wDay = day;
                sysTime.wHour = hour;
                sysTime.wMinute = minute;
                sysTime.wSecond = second;
                SetSystemTime(&sysTime);
            #else
                struct tm new_time;
                std::time_t now = std::time(nullptr);
                new_time.tm_sec = second;
                new_time.tm_min = minute;
                new_time.tm_hour = hour;
                new_time.tm_mday = day;
                new_time.tm_mon = month - 1;
                new_time.tm_year = year - 1900;
                new_time.tm_isdst = -1;

                if (std::mktime(&new_time) == -1) {
                    std::cerr << "Failed to set new time.\n";
                    return;
                }

                if (std::abs(std::difftime(now, std::mktime(&new_time))) < 2) {
                    std::cout << "Time has been set successfully.\n";
                } else {
                    std::cerr << "Failed to set new time.\n";
                }
            #endif
        }

    private:
        int m_year;
        int m_month;
        int m_day;
        int m_hour;
        int m_minute;
        int m_second;
};

/*
SystemTime sysTime; // 创建对象时自动读取当前系统时间
sysTime.setTime(2023, 3, 20, 12, 0, 0); // 设置新的系统时间
*/