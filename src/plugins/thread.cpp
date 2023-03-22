#include "thread.hpp"

#include <spdlog/spdlog.h>

namespace LightGuider{

    // 析构函数，停止所有线程并销毁 ThreadManager 对象
    ThreadManager::~ThreadManager() {
        joinAllThreads();
    }

    // 添加一个新线程，并指定线程名称
    // func：要执行的函数
    // name：线程名称
    void ThreadManager::addThread(std::function<void()> func, const std::string& name) {
        std::lock_guard<std::mutex> lock(m_mtx); // 线程锁
        m_threads.emplace_back(std::thread([func]() { // 添加线程
        try {
            func(); // 执行函数
        } catch (...) {
            spdlog::error("Unhandled exception in thread"); // 异常处理
        }
        }));
        m_threadNames.emplace_back(name); // 添加线程名
        m_sleepFlags.push_back(false); // 初始化睡眠标志
        spdlog::info("Added thread: {}", name); // 日志输出
    }

    // 等待所有线程完成并销毁 ThreadManager 对象
    void ThreadManager::joinAllThreads() {
        m_stopFlag = true; // 设置停止标志
        if (!m_threads.empty()) { // 如果 m_threads 不为空
        for (auto& t : m_threads) {
            t.join();
        }
        }
        spdlog::info("All threads joined"); // 日志输出
    }

    // 等待指定名称的线程完成，并从 ThreadManager 中移除该线程
    // name：线程名称
    void ThreadManager::joinThreadByName(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_mtx); // 线程锁
        for (size_t i = 0; i < m_threads.size(); ++i) {
        if (m_threadNames[i] == name) { // 找到要加入的线程
            m_threads[i].join(); // 加入线程
            m_threads.erase(m_threads.begin() + i); // 从容器中移除线程
            m_threadNames.erase(m_threadNames.begin() + i); // 从容器中移除线程名称
            spdlog::info("Thread {} joined", name); // 日志输出
            return;
        }
        }
        spdlog::warn("Thread {} not found", name); // 日志输出
    }

    // 让指定名称的线程休眠指定时间
    // name：线程名称
    // seconds：休眠时间（秒）
    // 返回值：如果找到了该线程，则返回 true；否则返回 false
    bool ThreadManager::sleepThreadByName(const std::string& name, int seconds) {
        std::lock_guard<std::mutex> lock(m_mtx); // 线程锁
        for (size_t i = 0; i < m_threads.size(); ++i) {
        if (m_threadNames[i] == name) { // 找到要休眠的线程
            m_sleepFlags[i] = true; // 设置睡眠标志
            std::this_thread::sleep_for(std::chrono::seconds(seconds)); // 让线程休眠
            m_sleepFlags[i] = false; // 取消睡眠标志
            spdlog::info("Thread {} slept for {} seconds", name, seconds); // 日志输出
            return true;
        }
        }
        spdlog::warn("Thread {} not found", name); // 日志输出
        return false;
    }

    // 检查指定名称的线程是否在运行
    // name：线程名称
    // 返回值：如果找到了该线程，则返回 true，表示该线程在运行；否则返回 false，表示该线程未找到
    bool ThreadManager::isThreadRunning(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_mtx); // 线程锁
        for (size_t i = 0; i < m_threads.size(); ++i) {
        if (m_threadNames[i] == name) { // 找到要检查的线程
            return !m_sleepFlags[i]; // 返回线程是否在运行
        }
        }
        spdlog::warn("Thread {} not found", name); // 日志输出
        return false;
    }

    ThreadManager ThreadManage;

}