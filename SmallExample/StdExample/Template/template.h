#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <deque>

template<class T>
T mymax(const T t1, const T t2)
{
}

template<class Ta, class Tb>
class stack
{
public:
    stack()
    {
        std::cout << "template<class Ta, class Tb>" << std::endl;
    }
    
    void Address()
    {
        static int nNumber = 0;
        printf("&nNumber = %p\n", &nNumber);
    }
};

