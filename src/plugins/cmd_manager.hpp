/*
 * cmd_manager.hpp
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
 
Description: Command Line Maneger
 
**************************************************/

#pragma once

#include <iostream>
#include <future>
#include <vector>
#include <string>
#include <mutex>
#include <array>
#include <stdexcept>
#include "spdlog/spdlog.h"

class CmdManager {
public:
    CmdManager() = default;
    ~CmdManager() = default;

    // 添加一个命令
    void AddCmd(const std::string& cmd) {
        std::lock_guard<std::mutex> guard(mutex_);
        cmds_.push_back(cmd);
    }

    // 执行所有命令，并返回它们的输出结果
    std::vector<std::string> RunAllCmds(bool print_ok_only = false) {
        spdlog::info("Start to execute all commands...");

        std::vector<std::future<std::string>> futures;
        for (auto& cmd : cmds_) {
            futures.emplace_back(std::async([cmd, this, print_ok_only](){
                // 获取不带参数的命令
                auto pos = cmd.find(" ");
                auto cmd_name = cmd.substr(0, pos != std::string::npos ? pos : cmd.size());

                if (!CheckCmdExist(cmd_name)) {
                    spdlog::error("Command '{}' not found!", cmd_name);
                    return std::string("");
                }

                std::array<char, 128> buffer;
                std::string result;
                int ret_code = -1;
                FILE* pipe = popen(cmd.c_str(), "r");
                if (!pipe) throw std::runtime_error("popen() failed!");
                while (!feof(pipe)) {
                    if (fgets(buffer.data(), 128, pipe) != NULL) {
                        result += buffer.data();
                    }
                }
                ret_code = pclose(pipe);

                if (ret_code == -1) {
                    spdlog::error("Failed to get the return code of command '{}'!", cmd);
                    return std::string("");
                } else if (ret_code != 0) {
                    spdlog::error("Command '{}' returned a non-zero code: {}!", cmd, ret_code);
                    if (print_ok_only) {
                        return std::string("");
                    }
                }
                
                spdlog::info("Command '{}' executed successfully!", cmd);
                return result;
            }));
        }

        std::vector<std::string> results;
        for (auto& future : futures) {
            results.push_back(future.get());
        }

        spdlog::info("All commands executed successfully!");
        return results;
    }

    // 清空所有命令
    void ClearCmds() {
        std::lock_guard<std::mutex> guard(mutex_);
        cmds_.clear();
    }

private:
    bool CheckCmdExist(const std::string& cmd) {
        std::string check_cmd = "command -v " + cmd + " >/dev/null 2>&1";
        int ret = system(check_cmd.c_str());
        return WEXITSTATUS(ret) == 0;
    }

    std::vector<std::string> cmds_;
    std::mutex mutex_;
};

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

// 异步子进程管理器
class AsyncCmdManager {
public:
    AsyncCmdManager() = default;
    ~AsyncCmdManager() = default;

    // 添加一个命令
    void AddCmd(const std::string& cmd) {
        cmds_.push_back(cmd);
    }

    // 执行所有命令，并返回它们的输出结果
    std::vector<std::string> RunAllCmds(bool print_ok_only = false) {
        std::vector<std::string> results;
        int pipefd[2];

        if (pipe(pipefd) == -1) {  // 创建管道
            std::cerr << "Failed to create pipe!" << std::endl;
            return results;
        }

        for (auto& cmd : cmds_) {
            auto pos = cmd.find(" ");
            auto cmd_name = cmd.substr(0, pos != std::string::npos ? pos : cmd.size());

            if (!CheckCmdExist(cmd_name)) {
                std::cerr << "Command '" << cmd_name << "' not found!" << std::endl;
                continue;
            }

            pid_t pid = fork();
            if (pid == -1) {
                std::cerr << "Failed to fork process!" << std::endl;
                continue;
            } else if (pid == 0) {  // 子进程中执行命令，并将输出通过管道传回
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                execl("/bin/sh", "sh", "-c", cmd.c_str(), nullptr);
                exit(1);
            }

            close(pipefd[1]);
            std::array<char, 128> buffer;
            std::string result;
            int ret_code = -1;

            while (true) {  // 读取子进程的输出
                int n = read(pipefd[0], buffer.data(), buffer.size());
                if (n <= 0) {
                    break;
                }
                result += buffer.data();
            }

            ret_code = waitpid(pid, NULL, 0);  // 等待子进程结束

            if (ret_code == -1) {
                std::cerr << "Failed to get the return code of command '" << cmd << "'!" << std::endl;
                continue;
            } else if (!WIFEXITED(ret_code) || WEXITSTATUS(ret_code) != 0) {
                std::cerr << "Command '" << cmd << "' returned a non-zero code: " << WEXITSTATUS(ret_code) << "!" << std::endl;
                if (print_ok_only) {
                    continue;
                }
            }

            results.push_back(result);
        }

        close(pipefd[0]);

        return results;
    }

    // 清空所有命令
    void ClearCmds() {
        cmds_.clear();
    }

private:
    bool CheckCmdExist(const std::string& cmd) {
        std::string check_cmd = "command -v " + cmd + " >/dev/null 2>&1";
        int ret = system(check_cmd.c_str());
        return WEXITSTATUS(ret) == 0;
    }

    std::vector<std::string> cmds_;
};

/*
    AsyncCmdManager manager;
    manager.AddCmd("echo Hello, world!");
    manager.AddCmd("ls /");
    manager.AddCmd("whoami");

    auto results = manager.RunAllCmds(true);

    for (auto& res : results) {
        std::cout << "Command output: " << std::endl;
        std::cout << res << std::endl;
    }
*/