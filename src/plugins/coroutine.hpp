#include <mutex>
#include <thread>
#include <coroutine>
#include <functional>
#include <string>
#include <chrono>
#include <atomic>
#include <vector>
#include <future>

class CoroutinePool {
public:
    // 构造函数
    CoroutinePool(int threadNum = std::thread::hardware_concurrency()) 
        : _threadNum(threadNum), _stopFlag(false)
    {}
    
    // 添加任务，并返回 future 对象
    template<typename T, typename F, typename... Args>
    auto addTask(F&& f, Args&&... args) -> std::future<T> {
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto fut = std::async(std::launch::async, [task]() {
            spdlog::debug("Coroutine starts");
            task();
            spdlog::debug("Coroutine finishes");
        });
        _futures.emplace_back(std::move(fut));
        return fut;
    }
    
    // 添加协程，并返回未来对象
    template<typename F, typename... Args>
    auto addCoroutine(F&& f, Args&&... args) -> std::future<void> {
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto fut = std::async(std::launch::async, [task]() {
            spdlog::debug("Coroutine starts");
            task();
            spdlog::debug("Coroutine finishes");
        });
        _futures.emplace_back(std::move(fut));
        return fut;
    }
    
    // 启动线程池并执行任务
    void start() {
        for (int i = 0; i < _threadNum; ++i) {
            _threads.emplace_back(std::bind(&CoroutinePool::runLoop, this));
        }
    }
    
    // 停止线程池
    void stopAll() {
        _stopFlag.store(true);
        for (auto& fut : _futures) {
            if (fut.valid()) {
                fut.wait();
            }
        }
    }
    
    // 动态结束指定的协程
    bool stopCoroutine(std::future<void>& fut) {
        if (fut.valid() && fut.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
            fut.wait();
            return true;
        } else {
            return false;
        }
    }
    
    // 判断指定的协程是否正在运行
    bool isCoroutineRunning(std::future<void>& fut) {
        return fut.valid() && fut.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
    }
    
    // 返回正在运行的协程数
    int runningCoroutineCount() {
        int count = 0;
        for (auto& fut : _futures) {
            if (isCoroutineRunning(fut)) {
                count++;
            }
        }
        return count;
    }

private:
    int _threadNum; // 线程池的线程数量
    std::atomic<bool> _stopFlag; // 停止标志，使用原子变量保证线程安全
    std::vector<std::thread> _threads; // 存储所有线程的 vector
    std::vector<std::future<void>> _futures; // 存储所有异步任务的 future 对象
    
    // 循环执行任务的方法
    void runLoop() {
        while (!_stopFlag.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};

void longLastingCalculation() {
    std::this_thread::sleep_for(std::chrono::seconds(5)); // 模拟一个长时间运行的任务
}

void newCoroutine() {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 模拟一个短时间运行的任务
}