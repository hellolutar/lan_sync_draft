#include "net_addr.h"

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
    return to_string(l_addr_) + ":" + to_string(l_port_);
}

sockaddr_in NetAddr::sockaddrV4() const
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(l_port_);
    addr.sin_addr.s_addr = htonl(l_addr_);

    return addr;
}
