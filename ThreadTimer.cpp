#include "ThreadTimer.h"
#include <boost/timer.hpp>
#include <boost/date_time.hpp>
#include <boost/pool/pool_alloc.hpp>

// ############################### class OnceTimer ###############################

// 单次定时操作
class OnceTimer : public std::enable_shared_from_this<OnceTimer>
{
    using TimerCallBack = std::function<void(uint64_t>;
public:
    // 从当前时刻延迟指定时间(单位：毫秒)
    explicit OnceTimer(boost::asio::io_service& ios, std::size_t ms, 
        TimerCallBack cb, uint64_t nTimerID)
        : m_timer(ios, boost::posix_time::milliseconds(ms))
        , m_cb(cb)
        , m_nTimerID(nTimerID)
    {
    }
    explicit OnceTimer(boost::asio::io_service& ios, 
        const boost::posix_time::ptime &tm, 
        TimerCallBack cb, uint64_t nTimerID)
        : m_timer(ios, tm)
        , m_cb(cb)
        , m_nTimerID(nTimerID)
    {
    }
    ~OnceTimer()
    {
        stop();
    }
    
    void start()
    {
        m_timer.async_wait(std::bind(&OnceTimer::on_timer,
            shared_from_this(), std::placeholders::_1));
    }
    void stop()
    {
        m_timer.cancel();
    }
    
protected:
    void on_timer(const boost::system::error_code& ec)
    {
        if (!ec)
        {
            m_cb(m_nTimerID);
        }
    }
    
private:
    boost::asio::deadline_timer m_timer;
    
    TimerCallBack m_cb;
    
    uint64_t m_nTimerID;
};

template<typename Ty>
using LIST_ALLOCATOR = boost::fast_pool_allocator<
    Ty,
    boost::default_user_allocator_new_delete>;
    
static LIST_ALLOCATOR<OnceTimer> s_alloc;

// ############################### class ThreadTimer ###############################

uint64_t MakeUniqueID()
{
    static const uint64_t nHigh = uint64_t(time(NULL)) << 32;
    
    static std::atomic<uint32_t> s_nInc { 0 };
    
    uint64_t nID = nHigh | (++s_nInc);
    
    return nID;
}

void ThreadTimer::Start()
{
    if (m_bRunning)
    {
        return ;
    }
    m_bRunning = true;
    
    AsyncTimerKeepLive();
    
    m_thread = boost::thread(boost::bind(&boost::asio::io_service::run, &m_ios));
}

void ThreadTimer::Stop()
{
    m_timerKeepLive.cancel();
    
    {
        // 停止所有时间未到的定时器
        std::unique_lock<std::mutex> lk(m_mutexTotalTimerDynamic);
        
        for (auto& mem : m_vecTotalTimerDynamic)
        {
            std::shared_ptr<OnceTimer> spTimer = mem.lock();
            if (spTimer)
                spTimer->stop();
        }
    }
    
    m_ios.stop();
    m_bRunning = false;
}

void ThreadTimer::AsyncTimerKeepLive()
{
    m_timerKeepLive.expires_from_now(boost::posix_timer::seconds(1));
    m_timerKeepLive.async_wait(std::bind(&ThreadTimer::DoTimerKeepLive, this));
}

void ThreadTimer::DoTimerKeepLive()
{
    m_timerKeepLive.expires_from_now(boost::posix_time::seconds(60));
    
    {
        // 添理所有已的定时器内存
        std::unique_lock<std::mutex> lk(m_mutexTotalTimerDynamic);
        
        std::vector<std::weak_ptr<OnceTimer>> vecTimerDynamic
            = std::move(m_vecTotalTimerDynamic);
            
        for (auto& mem : vecTimerDynamic)
        {
            std::shared_ptr<OnceTimer> spTimer = mem.lock();
            if (spTimer)
                m_vecTotalTimerDynamic.push_back(spTimer);
        }
    }
    
    m_timerKeepLive.async_wait(std::bind(&ThreadTimer::DoTimerKeepLive, this));
}

uint64_t ThreadTimer::AddExpiresFromNow(
    std::size_t ms, std::function<void(uint64_t)> cb)
{
    uint64_t nUniqueID = MakeUniqueID();
    
    std::shared_ptr<OnceTimer> spDelay
        = std::allocate_shared<OnceTimer>(
        s_alloc, m_ios, ms, cb, nUniqueID);
        
    spDelay->start();
    
    {
        std::unique_lock(std::mutex> lk(m_mutexTotalTimerDynamic);
        m_vecTotalTimerDynamic.push_back(spDelay);
    }
    
    return nUniqueID;
}

uint64_t ThreadTimer::AddExpiresFromAt(const std::string &fmtDateTime,
    std::size_t ms, std::function<void(uint64_t)> cb)
{
    // 斩仓超时时刻
    boost::posix_time::ptime tm(
        boost::posix_time::time_from_string(fmtDateTime));
        
    // 延迟超时时间
    tm += boost::posix_time::milliseconds(ms);
    
    // 转换为北京时间
    boost::posix_time::ptime local_tm = CH_ZONE::local_to_utc(tm);
    
    return AddExpiresAt(local_tm, cb);
}

uint64_t ThreadTimer::AddExpiresAt(
    const std::string &fmtDateTime,
    std::function<void(uint64_t)> cb)
{
    boost::posix_time::ptime tm(
        boost::posix_time::time_from_string(fmtDateTime));
        
    // 转换成北京时间
    boost::posix_time::ptime local_tm = CH_ZONE::local_to_utc(tm);
    
    return AddExpiresAt(local_tm, cb);
}

uint64_t ThreadTimer::AddExpiresAt(time_t atTime, std::function<void(uint64_t)> cb)
{
    boost::posix_time::ptime tm(
        boost::posix_time::from_time_t(atTime));
        
    return AddExpiresAt(tm, cb);
}

uint64_t ThreadTimer::AddExpiresAt(const boost::posix_time::ptime &tm, 
    std::function<void(uint64_t)> cb)
{
    uint64_t nUniqueID = MakeUniqueID();
    
    std::shared_ptr<OnceTimer> spDelay
        = std::allocate_shared<OnceTimer>(s_alloc, m_ios, tm, cb, nUniqueID);
        
    spDelay->Start();
    
    {
        std::unique_lock<std::mutex> lk(m_mutexTotalTimerDynamic);
        m_vecTotalTimerDynamic.push_back(spDelay);        
    }
    
    return nUniqueID;
}

std::size_t ThreadTimer::DynamicTimerSize()
{
    std::unique_lock(std::mutex> lk(m_mutexTotalTimerDynamic);
    
    std::vector<std::weak_ptr<OnceTimer>> vecTimerDynamic
        = std::move(m_vecTotalTimerDynamic);
        
    for (auto& mem : vecTimerDynamic)
    {
        std::shared_ptr<OnceTimer> spTimer = mem.lock();
        if (spTimer)
            m_vecTotalTimerDynamic.push_back(spTimer);
    }
    
    return m_vecTotalTimerDynamic.size();
}











