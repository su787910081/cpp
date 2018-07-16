#include "TestGuard.h"

#include <iostream>
#include <string>

void TestGuard::run()
{
    std::string line;
    
    do 
    {
        std::cout << std::endl;
        m_interpreter.print_key();
        std::cout << "input command: ";
        std::getline(std::cin, line);
        
        try
        {
            m_interpreter.parse_input(line);
        }
        catch (std::runtime_error& error)
        {
            std::cerr << error.what() << std::endl;
        }
    } while (line != "quit");
    
    std::cout << "test is quit" << std::endl;
}

void TestGuard::register_functions()
{
    // 这里将要测试的函数与字符串进行绑定，然后用输入的字符串与这些绑定的字符串进行匹配，
    // 如果匹配成功，则将后面输入的参数，传入到后面的函数中。
    m_iterpreter.register_function("RealQuote", this, &TestGuard::RealQuote);
    // ... 其他 的一些要测试的函数
}

void TestGuard::RealQuote()
{
    QuoteMI1::RealtimeQuote protoReq;
    
    uint32_t nGoodsID = 0;
    uint32_t nGoodsDecimal = 0;
    uint64_t nLastPrice = 0;
    uint64_t nNID = 0;
    
    std::cout << "nGoodsID, nGoodsDecimal, nLastPrice, NID: "; 
    std::cin >> nGoodsID >> nGoodsDecimal >> nLastPrice >> nNID;
    
    if (nLastPrice == 0)
        return;
}



