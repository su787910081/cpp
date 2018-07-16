#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "TransferHeader.pb.h"

class asio_connection
  : public boost::enable_shared_from_this<asio_connection>,
    public boost::noncopyable
{
public:
    asio_connection(boost::asio::io_service& io_svc, 
        boost::shared_ptr<boost::asio::ip::tcp::socket> sp_sock);
    virtual ~asio_connection();
    
public:
    void start(boost::function<void()> funcDisconnect);
    void stop();
    
    void async_read_header_size();
    void handle_header_size(
        const boost::system::error_code &ec,
        std::size_t bytes_transfered);
    void handle_header(const boost::system::error_code &ec,
        std::size_t bytes_transfered);
    void handler_head(
        std::shared_ptr<int> spLittleHeadSize, 
        const boost::system::error_code &ec, 
        std::size_t bytes_transfered);
        
    void writeTransfer(const char* topic, const std::string &bytesMsg);
    
private:
    bool verify(const boost::system::error_code &ec);
    void handle_write_header(const std::string &bytesHeader, const std::string &bytesMsg,
        const boost::system::error_code &ec, std::size_t bytes_transfered);
    void handle_write_subproto(const std::string &bytesMsg,
        const boost::system::error_code &ec, std::size_t bytes_transfered);
        
private:
    boost::asio::io_service& m_io_svc;
    boost::shared_ptr<boost::asio::ip::tcp::socket> m_sp_sock;
    int         m_nLitteleHeaderSize { 0 }; // 小端格式的消息头字节数
    std::string m_byteHeader;
    MsgHeader   m_protoHeader;
    boost::function<void()> m_funcDisconnect;   // 连接断开回调函数 
};
