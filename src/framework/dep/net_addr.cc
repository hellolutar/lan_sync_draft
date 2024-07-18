#include "net_addr.h"

#include <sstream>

using namespace std;

NetAddr::NetAddr(sockaddr_in addr, TransportType type)
{
    type_ = type;
    l_addr_ = ntohl(addr.sin_addr.s_addr);
    l_port_ = ntohs(addr.sin_port);
}

void NetAddr::setPort(uint16_t l_port) { l_port_ = l_port; }

TransportType NetAddr::type() const
{
    return type_;
}

void NetAddr::setType(TransportType t)
{
    type_ = t;
}

std::string NetAddr::str()
{
    return static_cast<const NetAddr &>(*this).str();
}
std::string NetAddr::str() const
{
    std::stringstream ss;
    std::string s1 = to_string(l_addr_ >> 24);
    std::string s2 = to_string(l_addr_ >> 16 & 255);
    std::string s3 = to_string(l_addr_ >> 8 & 255);
    std::string s4 = to_string(l_addr_ & 255);

    if (type_ == TransportType::TCP)
        ss << "tcp://";
    else
        ss << "udp://";
    ss << s1 << "." << s2 << "." << s3 << "." << s4 << ":" << to_string(l_port_);

    return ss.str();
}

sockaddr_in NetAddr::sockaddrV4() const
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(l_port_);
    addr.sin_addr.s_addr = htonl(l_addr_);

    return addr;
}
