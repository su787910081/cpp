
#include "asio_server.h"

int main()
{
    boost::asio::io_service         io_svc;
    boost::shared_ptr<asio_server>  sp_serv
        = boost::make_shared<asio_server>(io_svc, "", 20017);   // "" 空字符串表示监听本机所有IP
    sp_serv->listen(1);
        
    // 捕获信号
    boost::asio::signal_set signals_(io_svc);
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    signals_.async_wait([&io_svc](const boost::system::error_code ec, int sig)
    {
        LOG4CPLUS_INFO_FMT(gLog, "signal: %d, message: %s", sig, ec.message().c_str());
        io_svc.stop();
    });

    // 这里可以使用多个线程来处理这一个io_service 
    // 但是，不确定哪个线程处理哪一个唤醒
    // 也就是说当有消息到来时，io_service 随机找一个空闲线程，
    // 将收到的消息交给该处理线程处理
    // 这里只要有任务都 是交给这一个io_service 处理的，
    // 所以有效的连接阻塞和accept 阻塞是同等级别没有优先级。
    // 处理线程也是随机分配的
    std::vector<std::thread> vecThread;
    for (int i = 0; i < 5; ++i)
    {
        vecThread.emplace_back(std::thread([&io_svc]()
        {
            LOG4CPLUS_INFO_FMT(gLog, "thread start...");
            io_svc.run();
            LOG4CPLUS_INFO_FMT(gLog, "thread finish.");
        }));
    }
    
    for (size_t i = 0; i < vecThread.size(); ++i)
    {
        vecThread[i].join()
    }
    
    assert(io_svc.stopped());
    
#ifdef IN32
    system("pause");
#endif


    return 0;
}
