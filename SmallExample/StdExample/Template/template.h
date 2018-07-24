#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <deque>

// ==================================  模板函数  ==================================
template<class T>
T mymax(const T t1, const T t2)
{
}

// ==================================  class stack  ==================================

template<class Ta, class Tb>
class stack
{
public:
    stack()
    {
        std::cout << "template<class Ta, class Tb>" << std::endl;
    }
    
    int* Address()
    {
        // 模板是每个class 都 是不一样的，
        // 所以它们里面的static 对象也是各自一份不同的对象。
        // 但是模板的相同class static 对象则是一样的，如：
        // stack<char, char> s_ch1;
        // stack<char, char> s_ch2;
        // s_ch1.Address() == s_ch2.Address();
        static int nNumber = 0;
        
        printf("&nNumber = %p\n", &nNumber);
        
        return &nNumber;
    }
};

// bool 模板特化(所有模板参数 被指定)
template<>
class <stack<bool, char>
{
public:
    stack() { std::cout << "template<bool, char>" << std::endl; }
};

// 模板的偏特化(部分模板参数被指定)
templacte<class T>
class stack<bool, T>
{
public:
    stack() { std::cout << "template<bool, T>" << std::endl; }
};

// ==================================  不定参函数模板: f()  ==================================

// 不定参的模板参数 
template<class ... T>
void f(T ... args)
{
    std::cout << sizeof ... (args) << std::endl;
}

// ==================================  不定参函数模板: print()  ==================================

// 必须指定一个空参的模板函数，才能被正确递归。
// 否则到最后的调用将没有一个匹配的函数。
void print()
{
    std::cout << "empty" << std::endl;
}

// 普通模板函数 print
template<class T, class ... Args>
void print(T head, Args ... rest)
{
    if (sizeof ... (rest) == 0)
        return ;
    
    std::cout << "parameter " << typeid(T).name() << ": "
        << head << std::endl;
    
    print(rest ...);    // rest ... 将模板参数包依次解开，做为实参输入
}

// 第一个参数 为const char* 的偏特化模板函数 print
template<class ... Args>
void print(const char *head, Args ... rest)
{
    if (sizeof ... (rest) == 0)
        return ;
    
    std::cout << "parameter char* " << ": \"" << head << "\"" << std::endl;
    print(rest ...);
}

// ==================================  不定参函数模板: func  ==================================
// 不定参数的参数值解包方式: args ... (这样可以将不定参数的所有参数值按顺序一个一个展开)
// sizeof ... (args) <==> sizeof ... (Ts)
template<typename ... Ts>
void func(Ts ... args)
{
    const int size = sizeof ... (args) + 2;
    // func(1, 3, 4, 1.3);  // 1.3 截断成int
    // int res[4 + 2] = { 1, 1, 3, 4, 1(.3), 3 };
    int res[size] = { 1, args ..., 2 };
    // since initializer lists guarantee sequencing, this can be used to 
    // call a function on each element of a pack, in order: 
    // (std::cout << args, 0) ...
    // 这里使用了"," 逗号表达式，前面的一句被执行，然后结果被抛弃 。
    // 后面一句的结果作为了数组的初始化元素。
    int dummy[sizeof ... (Ts)] = { (std::cout << args << " ", args) ... };
    
    // std::cout << sizeof...(Ts) << std::endl;
    for (size_t i = 0; i < sizeof ... (Ts); ++i)
    {
        std::cout << "dummy[" << i << "]: " << dummy[i] << std::endl;
    }
}

// ==================================  不定参函数模板: funcChar()  ==================================

// 如果 模板参数 化列表 中没有给出参数 类型，
// 那么 编译器会继续 从函数 的参数 化列表 中去找参数 ，
// 然后推导出模板参数 化列表 中的参数 。
// eg: funcChar<'a', 'a', 'b', 't', 's'>(1, "dd", 2.1);
//      ==> sizeof ... varchars == 5;
//      ==> sizeof ... args == 3;
// 以下方式都 是编译不通过 的，似乎后面的不定参数，只能在函数 的实参列表 中给出。
// funcChar<'a', 'a', 'b', 't', 's', "dd", 2.1>("dd", 2.1);
// funcChar<'a', 'a', 'b', 't', 's', const char*, double>("dd", 2.1);
// funcChar<'a', 'a', 'b', 't', 's', "dd", 2.1>();
template<char ... varchars, typename ... Args>
void funcChar(Args ... args)
{
    char arrs[sizeof ... (varchars) + 1] = { varchars ... };
    
    arrs[sizeof ... (varchars)] = 0;
    
    std::cout << "arrs: \"" << arrs << "\"" << std::endl;
    std::cout << sizeof ... (Args) << std::endl;
}

// ==================================  class MyClass  ==================================

template<typename T1, typename T2>
class MyClass
{
public:
    MyClass()
    {
        printf("MyClass<T1, T2>: MyClass<%s, %s>\n", 
              typeid(T1).name(), typeid(T2).name());
    }
};

template<typename T>
class MyClass<T, T>
{
public:
    MyClass() 
    {
        printf("MyClass<T, T>: MyClass<%s, %s>\n",
              typeid(T).name(), typeid(T).name());
    }
};

template<typename T>
class MyClass<T, int>
{
public: 
    MyClass()
    {
        printf("MyClass<T, int>: MyClass<%s, int>\n", 
            typeid(T).name());
    }
};

template<typename T1, typename T2>
class MyClass<T1*, T2*>
{
public:
    MyClass()
    {
        printf("MyClass<T1*, T2*>: MyClass<%s, %s>\n", 
            typeid(T1).name(), typeid(T2).name());
    }
};

// ==================================  class MyCharClass  ==================================
template<char const* name>
class MyCharClass
{
public:
    MyCharClass()
    {
    }
};

// ==================================  class Stack: 模板的声明与实现分离  ==================================
template<typename T>
class Stack
{
private:
    std::deque<T> elems;
    
public:
    void push(T const& );
    void pop() {}
    T top() const { return elems.front(); }
    bool empty() const
    {
        return elems.empty();
    }
    
    template<typename T2>
    Stack<T>& operator = (Stack<T2> const&);
};

// 多模板参数的成员函数实现
#if 1
template<typename T>
template<typename T2>
#else
template<typename T, typename T2>   // 编译不通过 
#endif
Stack<T>& Stack<T>::operator =(Stack<T2> const& op2)
{
    if ((void *)this == (void *) &op2)
    {
        return *this);
    }
    
    Stack<T2> tmp(op2);
    
    elems.clear();
    while (!tmp.empty())
    {
        elems.push_front(tmp.top());
        tmp.pop();
    }
    
    return *this;
}

// ==================================  class List: 模板类里面的模板成员函数  ==================================
template<typename T>
class List
{
public:
    template<typename T2>   // 成员函数模板
    List(List<T2> const&);
};

template<typename T>
template<typename T2>
List<T>::List<List<T2> const& b)    // 位于类外部的成员函数模板定义
{
    // ...
}

template<typename T>
int length<List<T> const&); // 位于外部名字作用域的函数模板









