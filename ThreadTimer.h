#pragma once

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <mutex>
#include <memory>
#include <map>
#include <atomic>
#include <functional>

class OnceTimer;

class ThreadTimer
{
    public: 
        void Start();
        void Stop();
        
        void AsyncTimerKeepLine();
        /// 添加一个从当前时间延时指定时间操作，单位：毫秒。
        /// 返回定时器ID
        uint64_t AddExpiresFromNow(std::size_t ms, std::function<void(unit64_t)> cb);
        /// 从一个指定时刻延时指定时间的操作，单位：毫秒
        /// 返回定时器ID
        uint64_t AddExpiresFromAt(const std::string &fmtDateTime, 
            std::size_t ms, std::function<void(uint64_t)>cb);
        /// 添加一个指定时间处理的操作，只精确到秒
        /// fmtDateTime(YYYY-MM-DD HH:MM:SS)、(YYYYMMDDTHHMMSS)
        /// 返回定时器ID
        uint64_t AddExpiresAt(const std::string &fmtDateTime, 
            std::function<void(uint64_t)> cb);
        uint64_t AddExpiresAt(time_t atTime, std::function<void(uint64_t)> cb);
        uint64_t AddExpiresAt(const boost::posix_time::ptime &tm, 
            std::function<void(uint64_t)> cb);
        std::size_t DynamicTimerSize();
        
private:
    void DoTimerKeepLive();
    
public:
    boost::thread m_thread;
    
private:
    std::atomic<bool> m_bRunning { false };
    boost::asio::io_service m_ios;
    boost::asio::dealine_timer m_timerKeepLive { m_ios };
    mutable std::mutex m_mutexTotalTimerDynamic;
    std::vector<std::wear_ptr<OnceTimer>> m_vecTotalTimerDynamic;
};
