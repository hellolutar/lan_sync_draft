#include "not_found_exc.h"
#include "framework/net/net_addr.h"

std::ostream &operator<<(std::ostream &os, const NetAddr &addr)
{
    os << addr.l_addr_ << ":" << addr.l_port_;
    return os;
}
