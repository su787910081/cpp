#include "asio_client.h"

int main()
{
    boost::asio::io_service io_svc;
    
    for (int i = 0; i < 500; ++i)
    {
        boost::shared_ptr<asio_client client
            = boost::make_shared<asio_client>(io_svc, "127.0.0.1", 20017);
        client->async_connect();
    }
    
    std::vector<boost::thread> vecThread;
    for (int i = 0; i < 5; ++i)
    {
        vecThread.emplace_back(boost::thread(boost::bind(
            &boost::asio::io_service::run, &io_svc)));
    }
    
    for (int i = 0; i < vecThread.size(); ++i)
    {
        vecThread[i].join();
    }
    
    assert(io_svc, stopped());
    
    return 0;
}
