#pragma once

// 对测试解释器的应用示例

#include "test_interpreter.h"
#include <google/protobuf/message.h>

class TestGuard
{
private:
    test::interpreter m_interpreter;
    
public:
    TestGuard()
    {
        register_functions();
    }
    ~TestGuard()
    {
    }
    
public:
    void run();
    
private:
    void register_functions();
    void RealQuote();
};
