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
