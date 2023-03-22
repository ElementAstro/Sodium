#include <iostream>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <sys/types.h>
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // fork
#include <fcntl.h>      // open
#include <errno.h>      // errno
#include "spdlog/spdlog.h"  // 添加spdlog头文件

using namespace std;

class Apt {
public:
    Apt() {}

    bool update() {
        string cmd_str = "sudo apt-get update";
        return execute_cmd(cmd_str);
    }

    bool install(const string& package_name) {
        if (!is_valid_package_name(package_name)) {
            cout << "Invalid package name." << endl;
            return false;
        }

        string cmd_str = "sudo apt-get install " + package_name;
        return execute_cmd(cmd_str);
    }

    bool remove(const string& package_name) {
        if (!is_valid_package_name(package_name)) {
            cout << "Invalid package name." << endl;
            return false;
        }

        string cmd_str = "sudo apt-get remove " + package_name;
        return execute_cmd(cmd_str); 
    }

    bool search(const string& keyword, vector<pair<string, string>>& result) {
        if (!is_valid_keyword(keyword)) {
            cout << "Invalid keyword." << endl;
            return false;
        }

        string cmd_str = "sudo apt-cache search " + keyword;
        return execute_cmd(cmd_str, result); 
    }

    // 添加dpkg安装deb的功能
    bool install_deb(const string& deb_file_path) {
        // 检查是否有安装权限
        if (getuid() != 0) {
            cout << "No permission to install package.\n";
            return false;
        }

        // 获取deb包信息
        string cmd_str = "dpkg-deb -f " + deb_file_path;
        vector<pair<string, string>> result;
        execute_cmd(cmd_str, result);

        // 打印deb包信息
        for (auto& p : result) {
            spdlog::debug("{}: {}", p.first, p.second);
        }

        // 安装deb包
        cmd_str = "dpkg -i " + deb_file_path;
        return execute_cmd(cmd_str);
    }

private:
    bool execute_cmd(const string& cmd_str, vector<pair<string, string>>& result) {
        mutex mtx;              // 用于锁定结果向量，以避免多线程访问冲突
        bool ret = true;
        int pipe_fd[2];         // 用于创建管道
        pid_t pid;              // 进程PID
        char buffer[1024] = {0};         

        // 创建管道
        if (pipe(pipe_fd) == -1) {
            spdlog::error("Failed to create pipe: {}", strerror(errno));
            return false;
        }

        // 子进程执行系统命令，并将输出写入管道
        pid = fork();

        if (pid == -1) {
            spdlog::error("Failed to fork: {}", strerror(errno));
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            return false;
        } else if (pid == 0) {
            // 关闭读管道
            close(pipe_fd[0]);

            // 将标准输出、标准错误和标准输入重定向到管道写端
            dup2(pipe_fd[1], STDOUT_FILENO);
            dup2(pipe_fd[1], STDERR_FILENO);
            dup2(pipe_fd[1], STDIN_FILENO);

            // 执行命令
            system(cmd_str.c_str());

            // 关闭写管道
            close(pipe_fd[1]);

            exit(EXIT_SUCCESS);
        } else {
            // 关闭写管道
            close(pipe_fd[1]);

            // 从管道读取命令输出
            while (read(pipe_fd[0], buffer, sizeof(buffer)) > 0) {   
                string line(buffer);
                
                // 从每一行中提取软件名称和版本号，并加入结果向量
                stringstream ss(line);
                string name, version;
                ss >> name >> ws;
                getline(ss, version);

                lock_guard<mutex> lock(mtx);
                result.emplace_back(name, version);
            }

            // 关闭读管道
            close(pipe_fd[0]);

            // 等待子进程退出
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);

                if (exit_code != 0) {
                    spdlog::error("Command exited with non-zero status.");
                    ret = false;
                }
            } else {
                spdlog::error("Command was terminated abnormally.");
                ret = false;
            }
        }

        return ret;
    }

    // 原函数execute_cmd的重载，不返回结果
    bool execute_cmd(const string& cmd_str) {
        vector<pair<string, string>> result;
        return execute_cmd(cmd_str, result);
    }

    bool is_valid_package_name(const string& s) {
        // 包名只能包含字母、数字、连字符和加号，且必须以字母或数字开始和结束，长度在2到100之间
        if (s.length() < 2 || s.length() > 100) {
            return false;
        }
        
        if (!isalnum(s[0]) || !isalnum(s[s.length() - 1])) {
            return false;
        }
        
        for (int i = 1; i < s.length() - 1; i++) {
            if (!isalnum(s[i]) && s[i] != '-' && s[i] != '+') {
                return false;
            }
        }
        
        return true;
    }

    bool is_valid_keyword(const string& s) {
        // 关键词只能包含字母、数字、下划线和连字符，长度在2到100之间
        if (s.length() < 2 || s.length() > 100) {
            return false;
        }
        
        for (char c : s) {
            if (!isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        
        return true;
    }
};
