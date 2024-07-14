#include "net_addr.h"

using namespace std;

void NetAddr::setPort(uint16_t l_port)
{
    l_port_ = l_port;
}

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
    return static_cast<const NetAddr&>(*this).str();
}
std::string NetAddr::str() const
{
    return to_string(l_addr_) + ":" + to_string(l_port_);
}
