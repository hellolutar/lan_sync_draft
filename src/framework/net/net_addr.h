#ifndef __NET_ADDR_H_
#define __NET_ADDR_H_

#include <string>
#include <cstdint>

enum class TransportType
{
    TCP,
    UDP
};

class NetAddr
{
private:
    uint32_t l_addr_ = 0;
    uint16_t l_port_ = 0;
    TransportType type_;

public:
    NetAddr(TransportType type = TransportType::UDP) : type_(type) {}
    NetAddr(std::string ip_port, TransportType type = TransportType::UDP)
        : type_(type){};

    NetAddr(std::uint32_t l_addr, std::uint16_t l_port, TransportType type = TransportType::UDP)
        : l_addr_(l_addr), l_port_(l_port), type_(type){};

    NetAddr(const NetAddr &other)
        : l_addr_(other.l_addr_), l_port_(other.l_port_), type_(other.type_){};

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

    TransportType type() const;
};

#endif