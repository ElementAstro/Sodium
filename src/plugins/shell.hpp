#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

using namespace std;

class ShellExecutor {
public:
    // 构造函数，初始化spdlog日志模块
    ShellExecutor(const char *filename) {
        fp = fopen(filename, "r");

        // 初始化spdlog，输出到日志文件和控制台
        auto console_sink = spdlog::stdout_logger_mt("console");
        auto file_sink = spdlog::basic_logger_mt("file", "log.txt");
        logger = spdlog::get("console"); // 缺省使用控制台输出
        logger->set_level(spdlog::level::debug); // 设置日志级别为 debug
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
        logger->sinks().clear(); // 清空缺省的 sink
        logger->sinks().push_back(console_sink); // 添加 console_sink
        logger->sinks().push_back(file_sink); // 添加 file_sink
    }

    ~ShellExecutor() {
        if (fp != NULL) {
            fclose(fp);
        }
    }

    bool is_safe(const char *cmd) {
        // 列出不安全的命令列表，如关机、重启等
        const char *unsafe_cmds[] = {"shutdown", "reboot", "poweroff", NULL};
        for (int i = 0; unsafe_cmds[i] != NULL; i++) {
            if (strcmp(cmd, unsafe_cmds[i]) == 0) {
                return false;
            }
        }
        return true;
    }

    void execute() {
        if (fp == NULL) {
            return;
        }

        char buf[1024];
        while (fgets(buf, sizeof(buf), fp) != NULL) { // 逐行读取
            buf[strlen(buf) - 1] = '\0'; // 去掉行末换行符

            char *args[10]; // 存储参数的数组
            int argc = 0; // 参数个数

            // 将一行命令按空格拆分为参数
            char *p = strtok(buf, " ");
            while (p != NULL) {
                args[argc++] = p;
                p = strtok(NULL, " ");
            }
            args[argc] = NULL;

            // 检查是否安全
            if (!is_safe(args[0])) {
                logger->warn("The command {} is UNSAFE, skipped.", args[0]);
                continue;
            }

            // 执行命令
            logger->debug("Execute command: {}", buf);
            pid_t pid = fork();
            if (pid == 0) {
                execvp(args[0], args);
                exit(0);
            } else {
                wait(NULL); // 等待子进程结束
            }
        }

        fclose(fp); // 关闭文件
    }

private:
    FILE *fp;
    shared_ptr<spdlog::logger> logger;
};

/*
int main() {
    ShellExecutor executor("test.sh");
    executor.execute();

    return 0;
}

*/
