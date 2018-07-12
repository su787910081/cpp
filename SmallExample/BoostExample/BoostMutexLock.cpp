

// 使用boost::shared_mutex 做一个简单的读写锁

// 这样做出来的读写锁并不排队，有可能某个线程获取锁的时候失败了，然后进入休眠。
// 在休眠的时候另一线程也来获取同一把锁，但是它是可以成功的，这样后来的有可能会先获取到锁。
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

void UsageBoostLockWR()
{
    using BoostMutexWR = boost::shared_mutex;
    using BoostLockW = boost::unique_lock<BoostMutexWR>;
    using BoostLockR = boost::shared_lock<BoostMutexWR>;
    
    // 声明一个互斥量
    BoostMutexWR bstMtx;
    
    {
        // 获取读锁
        BoostLockR rLock(bstMtx);
    }
    
    {
        // 获取写锁
        BoostLockW wLock(bstMtx);
    }
}
