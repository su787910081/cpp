#include "asio_client.h"

extern log4cplus::Logger *gLog;

asio_client::asio_client(
    boost::asio::io_service& io_svc,
    const std::string &str_svr_ip,
    uint64_t svr_port)
    : m_io_svc(io_svc)
{
    if (str_svr_ip.empty())
        std::abort();
        
    using namespace boost::asio::ip;
    address_v4 addr(address_v4::from_string(str_svr_ip));
    m_svr_ep.address(addr);
    m_svr_ep.port(svr_port);
}

void asio_client::async_connect()
{
    LOG4CPLUS_INFO_FMT(*gLog, "async_connect waitting...");
    
    boost::shared_ptr<boost::asio::ip::tcp::socket> new_sock
        = boost::make_shared<boost::asio::ip::tcp::socket>(boost::ref(m_io_svc));
        
    boost::function<void(const boost::system::error_code &)> cb_connect = 
        boost::bind(&asio_client::handle_connect, shared_from_this(), new_sock, _1);
    new_sock->async_connect(m_svr_ep, cb_connect);
}

void asio_client::handle_connect(
    boost::shared_ptr<boost::asio::ip::tcp::socket> new_conn,
    const boost::system::error_code &ec)
{
    if (ec != 0)
    {
        LOG4CPLUS_INFO(*gLog, "connect failed: " << ec.message());
        return ;
    }
    LOG4CPLUS_INFO(*gLog, "connect success, server: " << new_conn->remote_endpoint());
    
    async_write(new_conn);
}

void asio_client::async_write(boost::shared_ptr<boost::asio::ip::tcp::socket> conn)
{
#if 0
    message MessageHead
    {
        optional uint32 FunCode = 1;
        optional uint32 RequestID       = 2;
        optional uint32 AccountID       = 3;
        optional uint32 AccessID        = 4;
        optional int64  ClientTime      = 5;
        optional uint32 GoodsID         = 6;
        optional bytes  UUID            = 7;
    }
#endif
    MessageHead pro;
    
    pro.set_funcode(9527);
    pro.set_requestid(10081);
    pro.set_accountid(49005);
    pro.set_accessid(1);
    pro.set_clienttime(time(NULL));
    pro.set_goodsid(35023);
    pro.set_uuid(std::string("uuid_500388"));
    
    std::shared_ptr<std::string> sp_data = createSharedString();
    if (!pro.SerializeToString(sp_data.get()))
    {
        LOG4CPLUS_ERROR_FMT(*gLog, "SerializeToString failed.");
        return ;
    }
    
    LOG4CPLUS_INFO_FMT(*gLog, "data.size() = %lld", sp_data->size());
    if (sp_data->size() == 0)
    {
        return ;
    }
    
    std::shared_ptr<head> sp_head = createSharedHead();
    sp_head->set_len_body((int32_t)sp_data->size());
    sp_head->serialize();
    
    boost::function<void(const boost::system::error_code &, std::size_t)> cb_write_head
        = boost::bind(&asio_client::handle_write_head,
            shared_from_this(), conn, sp_data, _1, _2);
    boost::asio::async_write(
        *conn, boost::asio::buffer(sp_head->get_data(), 
            head::HEAD_LENGTH), cb_write_head);
}

void asio_client::handle_write_head(
    boost::shared_ptr<boost::asio::ip::tcp::socket> conn,
    const std::shared_ptr<std::string> sp_data_proto,
    const boost::system::error_code &ec,
    std::size_t bytes_transfered)
{
    if (!conn->is_open())
    {
        LOG4CPLUS_INFO(*gLog, "socket was not opened.");
        return;
    }
    
    if (ec != 0)
    {
        if (ec == boost::asio::error::eof)
            LOG4CPLUS_INFO(*gLog, "Disconnect from " << conn->remote_endpoint());
        else
            LOG4CPLUS_INFO(*gLog, "Error on receive: " << ec.message());
        return;
    }
    
    boost::function<void(const boost::system::error_code &, std::size_t)> cb_write_proto
        = boost::bind(&asio_client::handle_write_proto, 
            shared_from_this(), conn, sp_data_proto, _1, _2);
    boost::asio::async_write(*conn, boost::asio::buffer(*sp_data_proto), cb_write_proto);
}

void asio_client::handle_write_proto(
    boost::shared_ptr<boost::asio::ip::tcp::socket> conn, 
    const std::shared_ptr<std::string> sp_data_proto,
    const boost::system::error_code &ec, 
    std::size_t bytes_transfered)
{
    if (!conn->is_open())
    {
        LOG4CPLUS_INFO(*gLog, "socket was not opened.");
        return;
    }
    
    if (ec != 0)
    {
        if (ec == boost::asio::error::eof)
            LOG4CPLUS_INFO(*gLog, "Disconnect from " << conn->remote_endpoint());
        else
            LOG4CPLUS_INFO(*gLog, "Error on receive: " << ec.message());
            
        return;
    }
    
    LOG4CPLUS_INFO(*gLog, "write proto finished.");
    
    // 数据写完了之后，可以继续读取对端发送过来的数据 。
    // 如果不再读对端的数据，直接返回则该socket 将会被断开。
    // async_read_headconn);
}


