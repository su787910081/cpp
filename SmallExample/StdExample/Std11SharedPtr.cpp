
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


// ============================== Usage ==============================

void funcUsage()
{
    // 实例化一个堆内存管理分配器，该分配器只能构造结构名为ClassNameXXX 的对象。
    LIST_ALLOCATOR<ClassNameXXX> m_alloc;  
    
    // 构造一个CassNameXXX 的共享指针对象，通过 m_alloc 分配器.
    // 调用构造函数为 ClassNameXXX(xxx, xxx)
    std::shared_ptr<ClassNameXXX> spInstance = 
        std::allocate_shared<ClassNameXXX>(m_alloc, xxx, xxx);
}

