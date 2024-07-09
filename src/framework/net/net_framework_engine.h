#ifndef __NET_FRAMEWORK_H_
#define __NET_FRAMEWORK_H_

#include <memory>

#include "framework/net/transport_proto.h"

class NetframeworkEngine
{
public:
    NetframeworkEngine() {}
    virtual ~NetframeworkEngine() {}
    virtual std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) = 0;
    virtual std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) = 0;
    virtual std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &addr) = 0;
    virtual std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &addr) = 0;
};

#endif