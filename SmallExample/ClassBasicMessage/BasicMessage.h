#pragma once

// ########################################## 转换玫举为底层类型  ##########################################
#ifdef BOOST_NO_CXX11_CONSTEXPR
// 玫举转换成玫举底层类型
#define toUType(e)                                  \
    (static_cast<                                   \
        std::underlying_type<                       \
            std::remove_reference<                  \
                std::remove_cv<decltype(e)>::type   \
            >::type                                 \
        >::type                                     \
    >(e))
    
// 用于模板
#define toUTypeT(e)                                         \
    (static_cast<                                           \
        typename std::underlying_type<                      \
            typename std::remove_reference<                 \
                typename std::remove_cv<decltype(e)>::type  \
            >::type                                         \
        >::type                                             \
    >(e))
    
#else
// 玫举转换成玫举底层类型
template<typename E>
constexpr auto toUType(E enumerator)->typename std::underlying_type<E>::type
{
    return static_cast<typename std::underlying_type<E>::type>(enumerator);
}
#endif

// ########################################## 分配器 ##########################################

// 对堆内存的内存管理结构
// 通过使用std::allocate_shared<ClassName>(...) 构造一个堆内存空间
// 交给该结构管理，堆内在释放的时候内存空间交还给该结构，并没有真正从物理内存上释放，
// 这样下次再构造该结构的时候，可以重复循环利用，减少了new 和delete 的开销。


#if USE_BOOST_POOL
#include <boost/pool/pool_alloc.hpp>  // for boost::pool_allocator

// 数组型容器分配器
template<typename Ty>
using VECTOR_ALLOCATOR = boost::pool_allocator<Ty>;
// boost::pool_allocator<Ty, boost::default_user_allocator_new_delete>;

// 链表型容器分配器
template<typename Ty>
using LIST_ALLOCATOR = boost::fast_pool_allocator<Ty>;

#else
#include <memory>  // for std::allocator

// 数组型容器分配器
template<typename Ty>
using VECTOR_ALLOCATOR = std::allocator<Ty>;

// 链表型容器分配器
template<typename Ty>
using LIST_ALLOCATOR = std::allocator<Ty>;

#endif

// ########################################## 接口基类 ##########################################

#include <memory>

/// 消息分发处理接口

class IHandlerMessage
{
public:
    virtual ~IHandlerMessage() {}
    virtual void on_dispatch() = 0; // 主线程分配消息线程
};
using IHandlerMessageSptr = std::shared_ptr<IHandlerMessage>;

/// 消息处理对象构造接口
class IHandlerMsgCreator
{
public:
    virtual ~IHandlerMsgCreator() {}
    virtual IHandlerMessageSptr create(const std::string &msg) = 0;
    virtual IHandlerMessageSptr create(const char* ptr, size_t sz) = 0;
};
using IHandlerMsgCreatorSptr = std::shared_ptr<IHandlerMsgCreator>;

// ########################################## 消息ID ##########################################

enum class eMsgid : int
{
    // ...
}

// ########################################## 业务处理框架基类 ##########################################
template<class Derived,
    const char *const& kszListentopic,
    eMsgid kListenMsgid, 
    typename MessageProto>
class MessageBasic : 
    public IHandlerMessage,
    public std::enable_shared_from_this<Derived>
{
public:
    /// 类型别名
    using BasicHandler = MessageBasic<Derived, kszListenTopic, 
        kListenMsgid, MessageProto>;
    /// 监听主题 
    static inline const char* const ListenTopic() { return kszListenTopic; }
    // 监听功能号
    static inline const int ListenMsgid() { return toUType(kListenMsgid); }

public:
    explicit MessageBasic(const std::string& msg)
    {
        if (!m_msgProto.ParseFromArray((const void*)msg.data(), msg.size()))
        {
            throw parse_failed();
        }
    }
    explicit MessageBasic(const char* ptr, size_t sz)
    {
        if (!m_msgProto.ParseFromArray((const void*)ptr, sz))
        {
            throw parse_failed();
        }
    }
    virtual ~MessageBasic()
    {
    }

    /// 业务分发线程处理函数 
    virtual void on_dispatch() override final
    {
        // TODO: 从业务分发线程，投递 到业务处理线程。
        // 在业务处理线程中自动调用OnBusiness 函数 
        // example：
        //ThreadTask::s_insMessage.AddTask(
        //      std::bind(&MessageBasic::OnBusiness, shared_from_this()));
    }
    
    // 业务处理线程处理函数，根据具体业务需要，由子类实现业务逻辑。
    virtual void OnBusiness() = 0;

private:
    class MsgCreator : public IHandlerMsgCreator
    {
    public:
        virtual IHandlerMessageSptr create(const std::string& msg) override final
        {
            try
            {
                return std::allocate_shared<Derived>(m_alloc, msg);
            }
            catch (const parse_failed&)
            {
                return nullptr;
            }
        }
        virtual IHandlerMessageSptr create(const char* ptr, size_t sz) override final
        {
            try
            {
                return std::allocate_shared<Derived>(m_alloc, ptr, sz);
            }
            catch (const parse_failed&)
            {
                return nullptr;
            }
        }
    private:
        LIST_ALLOCATOR<Derived> m_alloc;
    };

public:
    static MsgCreator s_msgCreator;
    
protected:
    struct parse_failed {};
    MessageProto m_msgProto;
};
    
/// 静态变量对象的实例化
template<class Derived, const char *const& kszListenTopic,
    eMsgid kListenMsgid, typename MessageProto> MsgCreator
MessageBasic<Derived, kszListenTopic, kListenMsgid, MessageProto>::s_msgCreator;




