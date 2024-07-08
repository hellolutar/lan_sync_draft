#ifndef __NET_ADDR_H_
#define __NET_ADDR_H_

#include <string>
#include <cstdint>

class NetAddr
{
private:
    uint32_t l_addr_ = 0;
    uint16_t l_port_ = 0;

public:
    NetAddr(/* args */) {}
    NetAddr(std::string ip_port);
    NetAddr(std::uint32_t l_addr, std::uint16_t l_port);
    NetAddr(const NetAddr &other);

    ~NetAddr() {}

    void setPort(uint16_t l_port);

    bool operator==(const NetAddr &other) const
    {
        return l_addr_ == other.l_addr_ && l_port_ == other.l_port_;
    }
    bool operator<(const NetAddr &other) const
    {
        return l_addr_ < other.l_addr_ || l_port_ < other.l_port_;
    }
};

#endif