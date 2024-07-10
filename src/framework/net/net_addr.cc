#include "net_addr.h"

#include <arpa/inet.h>
#include <stdlib.h>

using namespace std;

NetAddr::NetAddr(std::string ip_port, TransportType type = TransportType::UDP) : type_(type)
{
    uint64_t pos = ip_port.find(':');
    if (pos == 0)
    {
        l_addr_ = INADDR_ANY;
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

void NetAddr::setPort(uint16_t l_port)
{
    l_port_ = l_port;
}

TransportType NetAddr::type() const
{
    return type_;
}
