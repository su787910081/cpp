#include "asio_server.h"

#include <atomic>
#include "asio_connection.h"

#include "Log.h"

asio_server::asio_server(
    boost::asio::io_service &io_svc,
    const std::string &lis_ip, uint64_t lis_port)
    : m_io_svc(io_svc)
    , m_lis_ip(lis_ip)
    , m_lis_port(lis_port)
    , m_acceptor(io_svc)
{
    boost::asio::ip::address_v4 lis_addr;   // 默认构造是监听本机所有IP 及端口
    
    if (!m_lis_ip.empty())
    {
        // 如果 m_lis_ip == "" 的话，程序 调用from_string("") 会挂掉。
        lis_addr = boost::asio::ip::address_v4::from_string(m_lis_ip);
    }
    
    boost::asio::ip::tcp::endpoint lis_ep(lis_addr, m_lis_port);
    
    boost::asio::ip::tcp::acceptor acc(io_svc, lis_ep);
    m_acceptor = std::move(acc);
}

asio_server::~asio_server()
{
}

bool asio_server::listen(int accept_num /*= 1*/)
{
    if (accept_num <= 0)
        return false;
        
    for (int i = 0; i < accept_num; ++i)
    {
        async_accept();
    }
    
    return true;
}

void asio_server::async_accept()
{
    LOG4CPLUS_INFO_FMT(gLog, "async_accept waitting...");
    
    boost::shared_ptr<boost::asio::ip::tcp::socket> new_sock
        = boost::make_shared<boost::asio::ip::tcp::socket>(
            boost::ref(m_io_svc));
            
    boost::function<void(const boost::system::error_code &)> cb_accept;
    cb_accept = boost::bind(&asio_server::handle_accept,
        shared_from_this(), new_sock, _1);
        
    m_accpetor.async_accept(*new_sock, cb_accept);
}

void asio_server::handle_accept(
    boost::shared_ptr<boost::asio::ip::tcp::socket> new_sock,
    const boost::system::error_code &ec)
{
    if (ec != 0)
    {
        LOG4CPLUS_INFO(gLog, "accept failed: " << ec.message());
        return ;
    }
    
    LOG4CPLUS_INFO(gLog, "a new client connected." << new_sock->remote_endpoint());
    LOG4CPLUS_INFO_FMT(gLog, "current connect number: %d", ++m_nNumConnection);
    
    boost::shared_ptr<asio_connection> conn = 
        boost::make_shared<asio_connection>(m_io_svc, new_sock);
        
    auto self = shared_from_this();
    conn->start([self](){ self->disconnect(); });
    
    // 处理下一个连接，每次处理完了之后，需要再次accept.
    // 否则BOOST 将只处理一次，然后结束监听。
    // 所以这里可以处理一个情况，就是当你要结束监听的时候，只要在这里return
    // 那么io_service 的run() 函数就会stop. 但如果有其他的异步操作时，
    // run() 函数还是会继续运行的。
    async_accept();
}

void asio_server::disconnect()
{
    LOG4CPLUS_INFO_FMT(gLog, "current connect number: %d", --m_nNumConnection);
}




