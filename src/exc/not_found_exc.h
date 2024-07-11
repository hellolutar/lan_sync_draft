#ifndef __NOT_FOUND_EXC_H_
#define __NOT_FOUND_EXC_H_

#include <string>
#include <iostream>
#include <stdexcept>

class NotFoundException : public std::exception
{
protected:
    void print()
    {
    }

    template <typename T, typename... Args>
    void print(T first, Args... rest)
    {
        std::cerr << first;
        print(rest...);
    }

public:
    template <typename... Args>
    NotFoundException(Args... rest)
    {
        print(rest...);
        std::cerr << std::endl;
    }
    ~NotFoundException() {}
};

#endif