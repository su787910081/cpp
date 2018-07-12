

// 1. 普通  mutex 
#include <mutex>
void UsageMutex()
{
    // 首先需要声明一个mutex 的实例对象
    std::mutex mtx;
    
    // 使用的时候
    {
        // 独占锁 使用了C++ 的构造与析构特性 在同一个线程中不允许多次获取同一个锁
        // std::mutex不允许拷贝构造，也不允许 move 拷贝
        // 构造成功则获取锁；
        // 析构时自动释放锁；
        std::unique_lock<std::mutex> lk(mtx);
        // XXXX 对锁的对象做独占操作
        ...
    }
    
    // 上面是使用unique_lock 来对锁进行加解处理，std::mutex 也可以手动进行加解锁操作
    mtx.lock();
    // XXXX 对锁的对象做独占操作
    ...
    mtx.unlock();
}

// 2. 同一线程中可多次获取锁，但是对应的释放 次数需要与获取次数相同
// std::recursive_mutex与std::mutex一样，也是一种可以被上锁的对象，
// 但是，std::recursive_mutex允许同一个线程对互斥量多次上锁，即递归上锁，
// 来获得对互斥量的多层所有权，
// std::recursive_mutex：释放互斥量时需要调用与该锁层次深度相同的unlock，
// 可以理解为unlock次数相同，其它二者大致相同。

/*
 * std::time_mutex比std::mutex多了两个成员函数
 * 1）try_lock_for()：
 *      函数接受一个时间范围，表示在这一段时间范围内线程如果没有获得锁则被阻塞住（与std::mutex_lock_try()不同，
 *      try_lock如果被调用时，没有获得锁，则直接返回false，如果在此期间其它线程释放了锁，则该线程可以获得对互斥量的锁，
 *      如果超时(即在指定时间内还没有获得锁)，则返回false
 * 2）try_lock_until()：函数接收一个时间点作为参数，
 *      在指定时间点未到来之前，线程如果没有获得锁则被阻塞住，
 *      如果在此期间其它线程释放了锁，则该线程可以获得互斥量的锁，如果超时，则返回false
 */

// timed_mutex::try_lock_for example
#include <iostream>       // std::cout
#include <chrono>         // std::chrono::milliseconds
#include <thread>         // std::thread
#include <mutex>          // std::timed_mutex
 
std::timed_mutex mtx;
 
void fireworks () {
  // waiting to get a lock: each thread prints "-" every 200ms:
  while (!mtx.try_lock_for(std::chrono::milliseconds(200))) {
    std::cout << "-";
  }
  // got a lock! - wait for 1s, then this thread prints "*"
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "*\n";
  mtx.unlock();
}
 
int main ()
{
  std::thread threads[10];
  // spawn 10 threads:
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(fireworks);
 
  for (auto& th : threads) th.join();
 
  return 0;
}


