

// C++ 条件变量
// C++ 的条件变量是需要与mutex 配合才能使用的
#include <mutex>
#include <condition_variable>

void Usage()
{
    std::mutex mtx;
    std::condition_variable cond;
    
    // 在使用的时候必须要先成功获取到锁，然后将获取到的锁给到条件变量进入休眠 。
    // 在这个过程中条件变量会将该锁释放，然后再进入休眠。
    // 直到收到一个唤醒信号的时候才会从条件变量中出来，从条件变量中醒来的同时条件变量会先去获取锁，
    // 当成功获取到锁才会真正从条件变量出来
    // 所以进入条件变量需要先加锁，退出条件变量之后需要再解锁，这都是一对操作。
    {
        std::unique_lock<std::mutex> lk(mtx);
        cond.wait(lk);  // 释放锁后，进入休眠。   
        // 这个时候 只有等另外一个线程调用: cond.notify_all() 或者 cond.notify_one() 
        // 当前线程才有可能 从休眠 中醒来。
        // ... 对锁信的一段代码进行操作。
    }   // 结束 之后lk 析构，自动释放锁。
    
    // 条件变量除了在收到信号的时候会从条件变量中醒来以外，还可以为其设定一个时间，
    // 让其在到达指定时间的等待之后也从条件变量中醒来。
    
    {
        std::unique_lock<std::mutex> lk(mtx);
        cond.wait_for(lk, std::chrono::seconds(1));
        // ...
    }
}

