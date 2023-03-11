#include <iostream>
#include <string>
#include <sstream> //stringstream
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <exception>

// 下载类
class CDownload
{
public:
    CDownload() = delete;
    CDownload(int id, std::string &name)
        : mId(id), mName(name)
    {
        mIsPause = false;
        mIsFinish = false;
    }
    CDownload(const CDownload &src)
    {
        // std::cout << "copy..." << std::endl;
        mId = src.mId;
        mName = src.mName;
        mIsPause = false;
        mIsFinish = false;
    }
    ~CDownload()
    {
        if (mThread.joinable())
        {
            mThread.join();
        }
    }
    // 线程启动
    void Run()
    {
        mThread = std::thread{&CDownload::ProcessEntries, this};
    }
    // 下载函数
    void ProcessEntries()
    {
        while (!mIsFinish)
        {
            // std::lock_guard<std::mutex> lock(mMutex);
            if (mIsPause)
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << "线程" << mId << " 正在暂停... " << std::endl;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << "线程" << mId << " 正在下载... " << std::endl;
                // 假如3s下载完成
                std::this_thread::sleep_for(std::chrono::seconds(3));
                std::cout << "线程" << mId << " 下载完成... " << std::endl;
                mIsFinish = true;
            }
        }
    }
    // 暂停
    void Pause() { mIsPause = true; }
    // 唤醒
    void Resume() { mIsPause = false; }

private:
    int mId;
    std::string mName;
    bool mIsPause;
    bool mIsFinish;
    std::thread mThread;
    static std::mutex mMutex;
};

std::mutex CDownload::mMutex;

// 下载管理类
class CDownloadMgr
{
public:
    CDownloadMgr() {}
    ~CDownloadMgr() {}
    // 创建新下载
    void CreateDownload(CDownload &NewDownload)
    {
        mVthread.push_back(NewDownload);
        (mVthread.end() - 1)->Run();
    }
    // 创建新下载
    void CreateDownload(int id, std::string &name)
    {
        CDownload Download(id, name);
        mVthread.push_back(Download);
        (mVthread.end() - 1)->Run();
    }
    // 暂定指定线程
    void PauseOne(int nIndex)
    {
        if (nIndex >= mVthread.size())
            throw std::runtime_error("暂定指定线程出错");
        mVthread.at(nIndex).Pause();
    }
    // 唤醒指定线程
    void ResumeOne(int nIndex)
    {
        if (nIndex >= mVthread.size())
            throw std::runtime_error("唤醒指定线程出错");
        mVthread.at(nIndex).Resume();
    }
    // 全部暂停
    void PauseALL()
    {
        for (auto &t : mVthread)
        {
            t.Pause();
        }
    }
    // 全部开始
    void StartALL()
    {
        for (auto &t : mVthread)
        {
            t.Resume();
        }
    }

private:
    std::vector<CDownload> mVthread;
};
int main()
{
    CDownloadMgr DownloadMgr;
    int n = 10;
    for (int i = 0; i < n; ++i)
    {
        std::stringstream ss;
        ss << " 线程" << i << "名称";
        DownloadMgr.CreateDownload(i, ss.str());
    }
    return 0;
}