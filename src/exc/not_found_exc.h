#ifndef __NOT_FOUND_EXC_H_
#define __NOT_FOUND_EXC_H_

#include <string>
#include <iostream>
#include <stdexcept>

// template<typename T>
// auto operator<<(std::ostream &os, const T &t) -> decltype(t.str(os), os)
// {
//     os << t.str();
//     return os;
// }

class NotFoundException : public std::exception
{
protected:
    void print()
    {
    }

    template <typename T, typename... Args>
    void print(T first, Args... rest)
    {
        std::cout << first;
        // print(rest...); todo
    }

public:
    template <typename... Args>
    NotFoundException(Args... rest)
    {
        std::cout << "NotFoundException:";
        print(rest...);
        std::cout << std::endl;
    }
    ~NotFoundException() {}
};

#endif