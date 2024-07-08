#include "net_addr.h"

#include <arpa/inet.h>
#include <stdlib.h>

using namespace std;

NetAddr::NetAddr(std::string ip_port)
{
    uint64_t pos = ip_port.find(':');
    if (pos == 0)
    {
        l_addr_= INADDR_ANY;
    }
    else
    {
        string ip = ip_port.substr(0, pos);
        sockaddr_in tmp_addr;
        inet_aton(ip.data(), &(tmp_addr.sin_addr));
        l_addr_ = ntohl(tmp_addr.sin_addr.s_addr);
    }
    if (ip_port.find(":") != string::npos)
    {
        string port = ip_port.substr(pos + 1, ip_port.size());
        l_port_ = stoi(port);
    }
}

NetAddr::NetAddr(std::uint32_t addr, std::uint16_t port)
{
    l_addr_ = addr;
    l_port_ = port;
}

NetAddr::NetAddr(const NetAddr &other)
{
    l_addr_ = other.l_addr_;
    l_port_ = other.l_port_;
}

void NetAddr::setPort(uint16_t l_port)
{
    l_port_ = l_port;
}
