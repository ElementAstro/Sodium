/*
 * file_monitor.hpp
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

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <sys/inotify.h>

class FileMonitor {
    public:
        FileMonitor() : fd_(inotify_init()) {
            if (fd_ < 0) {
                std::cerr << "inotify_init failed: " << std::strerror(errno) << "\n";
                exit(1);
            }
        }

        ~FileMonitor() {
            for (auto wd : wds_) {
                inotify_rm_watch(fd_, wd);
            }
            close(fd_);
        }

        void add_file(const std::string& file_path) {
            int wd = inotify_add_watch(fd_, file_path.c_str(), IN_MODIFY);
            if (wd < 0) {
                std::cerr << "inotify_add_watch failed for " << file_path << ": " << std::strerror(errno) << "\n";
                return;
            }
            std::cout << "Start to watch file " << file_path << "...\n";
            wds_.push_back(wd);
            file_paths_.push_back(file_path);
        }

        void remove_file(const std::string& file_path) {
            auto it = std::find(file_paths_.begin(), file_paths_.end(), file_path);
            if (it != file_paths_.end()) {
                int index = std::distance(file_paths_.begin(), it);
                int wd = wds_[index];
                inotify_rm_watch(fd_, wd);
                wds_.erase(wds_.begin() + index);
                file_paths_.erase(it);
                std::cout << "Stop watching file " << file_path << "...\n";
            }
        }

        void start() {
            char buffer[EVENT_BUF_LEN];
            while (true) {
                int length = read(fd_, buffer, EVENT_BUF_LEN);
                if (length < 0) {
                    std::cerr << "read failed: " << std::strerror(errno) << "\n";
                    break;
                }

                int i = 0;
                while (i < length) {
                    struct inotify_event *event = (struct inotify_event *) &buffer[i];
                    if (event->len) {
                        if (event->mask & IN_MODIFY) {
                            std::string file_path;
                            for (const auto& path : file_paths_) {
                                if (event->wd == inotify_add_watch(fd_, path.c_str(), IN_MODIFY)) {
                                    file_path = path;
                                    break;
                                }
                            }

                            if (file_path.empty()) {
                                // should not happen
                                std::cerr << "Internal error: cannot find file path for watch descriptor " << event->wd << "\n";
                                break;
                            }

                            std::ifstream file(file_path);
                            if (!file.is_open()) {
                                std::cerr << "Failed to open file " << file_path << ": " << std::strerror(errno) << "\n";
                                break;
                            }
                            int offset = 0;
                            // 定位到修改的位置
                            #ifdef __NR_inotify_init1
                                offset = sizeof(struct inotify_event) + event->len;
                            #else
                                offset = event->off;
                            #endif
                            file.seekg(offset, std::ios::beg);

                            // 读取修改位置后的内容
                            std::string line;
                            while (std::getline(file, line)) {
                                std::cout << line << "\n";
                            }
                        }
                    }
                    i += EVENT_SIZE + event->len;
                }
            }
        }

    private:
        static const int EVENT_SIZE = sizeof(struct inotify_event);
        static const int EVENT_BUF_LEN = 1024 * (EVENT_SIZE + 16);

        int fd_;
        std::vector<int> wds_;
        std::vector<std::string> file_paths_;
};

/*
    FileMonitor monitor;

    // 监听文件1和文件2的修改事件
    monitor.add_file("path/to/file1");
    monitor.add_file("path/to/file2");

    // 开始监视
    monitor.start();

    // 停止监听文件2的修改事件
    monitor.remove_file("path/to/file2");
*/