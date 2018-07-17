#pragma once

#include <string>
#include <cstdint>

#include <boost/asio.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <common/pools.h>

class asio_client : public boost::enable_shared_from_this<asio_client>,
                    public boost::noncopyable
{
public:
    asio_client(boost::asio::io_service& io_svc, 
        const std::string &str_svr_ip, uint16_t svr_port);
    ~asio_client() = default;
    
    void async_connect();
    
protected:
    void handle_connect(boost::shared_ptr<boost::asio::ip::tcp::socket> new_conn, 
        const boost::system::error_code &ec);
        
    void async_write(boost::shared_ptr<boost::asio::ip::tcp::socket> conn);
    
    void handle_write_head(boost::shared_ptr<boost::asio::ip::tcp::socket> conn, 
        const std::shared_ptr<std::string> sp_data_proto,
        const boost::system::error_code &ec,
        std::size_t bytes_transfered);
        
    // 这里的sp_data_proto 在该函数中并不需要使用，
    // 用它作参数的唯一作用，就是保留它的生命周期， 
    // 保证在数据写完之前它不会被析构。
    // 因为，如果该对象在async_write 还未写之前就被析构的话，
    // 就会造成数据的错乱，最终导致对端接收到的数据是错误的。
    void handle_write_proto(boost::shared_ptr<boost::asio::ip::tcp::socket> conn,
        const std::shared_ptr<std::string> sp_data_proto,
        const boost::system::error_code &ec, 
        std::size_t bytes_transfered);
        
private:
    boost::asio::io_service&            m_io_svc;
    boost::asio::ip::tcp::endpoint      m_svr_ep;
};

