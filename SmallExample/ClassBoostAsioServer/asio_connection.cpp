#include "asio_connection.h"

#include <boost/endian/conversion.hpp>

#include "InnerFunc.h"
#include "Log.h"

#define HEAD_LENGTH sizeof(int)

/// @def FUNCODE_LEN
/// @brief 功能号长度
#define FUNCODE_LEN  4

/// @def SESSIONID_LEN
/// @brief 
#define SESSIONID_LEN   4

#define MSG_HEAD_LEN (FUNCODE_LEN + SESSIONID_LEN)

asio_connstion::asio_connection(boost::asio::io_service& io_svc,
    boost::shared_ptr<boost::asio::ip::tcp::socket> sp_sock)
    : m_io_svc(io_svc)
    , m_sp_sock(sp_sock)
{
}

asio_connection::~asio_connection()
{
    stop();
    
    if (m_funcDisconnect)
        m_funcDisconnect();
}

void asio_connection::start(boost::function<void()> funcDisconnect)
{
    m_funcDisconnect = funcDisconnect;
    async_read_header_size();
}

void asio_connection::stop()
{
    if (m_sp_sock->is_open())
        m_sp_sock->shutdown(boost::asio::socket_base::shutdown_both);
        
    LOG4CPLUS_INFO(gLog, "stop remote " << m_sp_sock->remote_endpoint());
}

void asio_connection::async_read_header_size()
{
    // 回调函数 
    boost::function<void(const boost::system::error_code &, std::size_t)> cb_msg_head;
    
    cb_msg_head = boost::bind(&asio_connection::handle_header_size, shared_from_this(), _1, _2);
        
    // 异步读，读一个报文的长度，boost::asio::async_read() 函数 有个特点，
    // 它会将这里指定的bbuffer 缓冲区读满了才会去回调handle_head 函数 
    boost::asio::async_read(
        *m_sp_sock, boost::asio::buffer(&m_nLittleHeaderSize, HEAD_LENGTH);
}

void asio_connection::handle_header_size(
    const boost::system::error_code &ec, 
    std::size_t bytes_transfered)
{
    if (!verify(ec))
        return;
        
    // 这里对接收到的数据做处理
    assert(bytes_transfered == HEAD_LENGTH);
    int nNativeHeadSize = boost::endian::little_to_native<int>(m_nLittleHeaderSize);
    LOG4CPLUS_INFO_FMT(gLog, "nLenLoc: %d", nNativeHeadSize);
    
    m_bytesHeader.resize(nNativeHeadSize);
    
    // 回调函数 
    boost::function<void(const boost::system::error_code &, std::size_t)> cb_proto;
    
    cb_proto = boost::bind(&asio_connection::handle_header, shared_from_this(), _1, _2);
    
    // 异步读，读一个报文的长度，boost::asio::async_read() 函数 有个特点，
    // 它会将这里指定的bbuffer 缓冲区读满了才会去回调handle_head 函数 
    boost::asio::async_read(
        *m_sp_sock, boost::asio::buffer(&m_byteHeader[0], nNativeHeadSize);
}

void asio_connection::handle_header(
    const boost::system::error_code &ec, 
    std::size_t bytes_transfered)
{
    if (!verify(ec))
        return;
        
    if (!m_protoHeader.ParseFromString(m_byteHeader))
    {
        LOG4CPLUS_ERROR_FMT(gLog, "ParseFromString failed.");
        return;
    }
    
    SendToBuf(m_protoHeader);
    
    async_read_header_size();
}

bool asio_connection::verify(const boost::system::error_coe &ec)
{
    if (!m_sp_sock->is_open())
    {
        LOG4CPLUS_INFO(gLog, "socket was not opened.");
        return false;
    }
    
    if (ec != 0)
    {
        if (ec == boost::asio::error::eof)
            LOG4CPLUS_INFO(gLog, "Disconnect from " << m_sp_sock->remote_endpoint());
        else
            LOG4CPLUS_INFO(gLog, "Error on receive: " << ec.message());
            
        return false;
    }
    
    return true;
}

void asio_connection::writeTransfer(const char* topic, const std::string &bytesMsg)
{
    const char* strMsg = bytesMsg.data();
    
    int nFuncID = 0;
    int nSessID = 0;
    memcpy(&nFuncID, strMsg, FUNCODE_LEN);
    memcpy(&nSessID, strMsg + FUNCODE_LEN, SESSIONID_LEN);
    MsgHeader destMsg;
    destMsg.set_funcid(nFuncID);
    destMsg.set_sessid(nSessID);
    destMsg.set_topic(topic);
    destMsg.set_subprotosize(int32_t(bytesMsg.size() - MSG_HEAD_LEN));
    
    std::string byteHeader;
    if (!destMsg.SerializeToString(&byteHeader))
    {
        LOG4CPLUS_ERROR_FMT(gLog, "SerializeToString failed.);
        return;
    }
    
    boost::function<void(const boost::system::error_code &, std::size_t)> cb_write_header
        = boost::bind(&asio_connection::handle_write_header, shared_from_this(), 
            boost::ref(byteHeader), boost::ref(bytesMsg), _1, _2);
    boost::asio::async_write(*m_sp_sock, boost::asio::buffer(byteHeader), cb_write_header);
}

void asio_connection::handle_write_header(
    const std::string &byteHeader,
    const std::string &bytesMsg,
    const boost::system::error_code &ec, 
    std::size_t bytes_transfered)
{
    if (!verify(ec)
    {
        return;
    }
    const char *subProtoData = bytesMsg.data() + MSG_HEAD_LEN;
    size_t nSubSize = bytesMsg.size() - MSG_HEAD_LEN;
    boost::function<void(const boost::system::error_code &, std::size_t)> cb_write_header
        = boost::bind(&asio_connection::handle_write_subproto, shared_from_this(), 
            boost::ref(bytesMsg), _1, _2);
    boost::asio::async_write(
        *m_sp_sock, boost::asio::buffer(subProtoData, nSubSize), cb_write_header);
}

void asio_connection::handle_write_subproto(
    const std::string &bytesMsg, 
    const boost::system::error_code &ec, 
    std::size_t bytes_transfered)
{
    if (!verify(ec))
    {
        return ;
    }
}



