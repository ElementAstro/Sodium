#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <set>
#include <string>
#include <vector>
#include <functional>

namespace LightGuider{
    class ThreadManager {
        public:
            ~ThreadManager();
            // 添加线程并启动
            void addThread(std::function<void()> func, const std::string& name);
            // 结束所有线程
            void joinAllThreads();
            // 结束指定名称的线程
            void joinThreadByName(const std::string& name);

            bool sleepThreadByName(const std::string& name, int seconds);

            bool isThreadRunning(const std::string& name);
        private:

            bool isThreadNameExist(const std::string& name) const {
                return std::find(m_threadNames.begin(), m_threadNames.end(), name) != m_threadNames.end();
            }

            std::vector<std::thread> m_threads;  // 线程列表
            std::vector<std::string> m_threadNames;  // 线程名称列表
            std::vector<bool> m_sleepFlags;
            std::mutex m_mtx;  // 互斥量，保护线程列表
            std::atomic<bool> m_stopFlag{ false };
    };

    extern ThreadManager ThreadManage;

}