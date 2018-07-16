#pragma once

#include <thread>
#include <vector>
#include <string>
#include <cstdint>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

class asio_connection;

class asio_server
    : public boost::enable_shared_from_this<asio_server>,
      public boost::noncopyable
{
public:
    asio_server(boost::asio::io_service &io_svc,
        const std::string &lis_ip, uint64_t lis_port);
        
    virtual ~asio_server();
    
    // 投放到io_service 中accept 的个数，一般多线程处理的时候 这里可以指定为多个，
    // 正常应该比线程个数少。
    bool listen(int accept_num = 1);
    
    void sendTransfer(const char* topic, const std::string &bytesMsg);
    
private:
    void async_accept();
    void handle_accept(boost::shared_ptr<boost::asio::ip::tcp::socket> new_sock, 
        const boost::system::error_code &ec);
        
    void disconnect();
    
private:
    boost::asio::io_service&                m_io_svc;
    std::string                             m_lis_ip;
    uint64_t                                m_lis_port;
    boost::asio::ip::tcp::acceptor          m_acceptor;
    boost::weak_ptr<asio_connection>        m_transfer_conn;
    std::atomic<int32_t>                    m_nNumConnection { 0 };
};

