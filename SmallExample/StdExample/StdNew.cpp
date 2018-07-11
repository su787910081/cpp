#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>

class tempTest
{
 tempTest() {}
 ~tempTest() {}
};

using tempPool = boost::singleton_pool<struct tempTestTag, sizeof(tempTest)>;

std::shared_ptr<tempTest> createSharedPtr()
{
 std::shared_ptr<tempTest> spTp(new (tempPool::malloc()) tempTest, 
 [](tempTest *tp)
 {
 tp->~tempTest();
 tempPool::free(tp);
 });

 return spTp;
}

void func()
{

 // 要点一: 使用new 关键字从一个已有内存块上构造一个类对象
 // 但是使用这个方法new 出来的对象，系统是不会主动调用析构函数的
 // 需要调用者自己使用析构函数进行释放。
 {
 tempTest* tp = new (tempPool::malloc()) tempTest;
 tp->~tempTest(); // 手动调用析构函数
 tempPool:free(tp);
 tp = nullptr;
 }

 // 要点二: 使用new 关键字与std::shared_ptr<> 关联使用
 {
 std::shared_ptr<tempTest> spTp(new (tempPool::malloc()) tempTest, 
 [](tempTest *tp)
 {
 tp->~tempTest();
 tempPool::free(tp);
 tp = nullptr;
 })
 }

 // 要点三：用这种方法生成的std::shared_ptr 是不能使用std::make_shared 
 // 模板来生成一个std::shared_ptr 对象的，只能通过std::shared_ptr<> 的
 // 两个参数的构造函数来进行创建，所以一般都需要一个单独的函数。
 {
 std::shared_ptr<tempTest> spTp;
 spTp = createSharedPtr();
 }

}
