/*
 * proc_manager.hpp
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
 
Description: Process Info
 
**************************************************/

#pragma once

#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
// For Windows
#include <windows.h>
#include <tlhelp32.h>
#else
// For Linux
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <signal.h>
#endif

class ProcessInfo {
    public:
        struct Info {
            int pid;
            std::string name;
            unsigned long long rss;
            unsigned long long vms;
            int cpu_percent;
        };

        ProcessInfo() {}

        std::vector<Info> getProcessList() {
            std::vector<Info> result;

    #if defined(_WIN32) || defined(_WIN64)
            // For Windows
            HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hSnap == INVALID_HANDLE_VALUE) {
                std::cerr << "Failed to create process snapshot: " << GetLastError() << std::endl;
                return result;
            }

            PROCESSENTRY32 pe32 = { sizeof(pe32) };
            if (!Process32First(hSnap, &pe32)) {
                std::cerr << "Failed to get first process: " << GetLastError() << std::endl;
                CloseHandle(hSnap);
                return result;
            }

            do {
                Info info = { pe32.th32ProcessID, pe32.szExeFile, 0, 0, 0 };
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                if (hProcess != NULL) {
                    PROCESS_MEMORY_COUNTERS pmc = { sizeof(pmc) };
                    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                        info.rss = pmc.WorkingSetSize;
                        info.vms = pmc.PagefileUsage;
                    }
                    CloseHandle(hProcess);
                }

                result.push_back(info);
            } while (Process32Next(hSnap, &pe32));

            CloseHandle(hSnap);
    #else
            // For Linux
            DIR* dir = opendir("/proc");
            if (!dir) {
                std::cerr << "Failed to open /proc directory" << std::endl;
                return result;
            }

            dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                int pid = atoi(entry->d_name);
                if (pid > 0) {
                    char path[256];
                    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

                    int fd = open(path, O_RDONLY);
                    if (fd >= 0) {
                        char buf[2048];
                        int len = read(fd, buf, sizeof(buf));
                        if (len > 0) {
                            buf[len] = '\0';
                            char* p = buf;
                            for (int i = 0; i < 22 && p != NULL; i++) {
                                p = strchr(p, ' ') + 1;
                            }
                            if (p != NULL) {
                                Info info = { pid, entry->d_name, 0, 0, 0 };
                                sscanf(p, "%*c %*d %*d %*d %*d %*d %*d %*d %*u %*u %*u %d %lld %lld", &info.cpu_percent, &info.rss, &info.vms);
                                result.push_back(info);
                            }
                        }
                        close(fd);
                    }
                }
            }

            closedir(dir);
    #endif

            return result;
        }

        bool killProcess(int pid) {
    #if defined(_WIN32) || defined(_WIN64)
            // For Windows
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            if (hProcess == NULL) {
                std::cerr << "Failed to open process " << pid << ": " << GetLastError() << std::endl;
                return false;
            }

            bool result = TerminateProcess(hProcess, 0) == TRUE;
            CloseHandle(hProcess);

            return result;
    #else
            // For Linux
            return kill(pid, SIGTERM) == 0;
    #endif
        }
};

/*
    ProcessInfo pi;
    std::vector<ProcessInfo::Info> list = pi.getProcessList();
    for (const auto& info : list) {
        std::cout << "- " << info.pid << ": " << info.name << ", RSS=" << info.rss << ", VMS=" << info.vms << ", CPU%=" << info.cpu_percent << std::endl;
    }

    std::cout << std::endl;

    if (pi.killProcess(1234)) {
        std::cout << "Process 1234 killed" << std::endl;
    } else {
        std::cerr << "Failed to kill process 1234" << std::endl;
    }
*/