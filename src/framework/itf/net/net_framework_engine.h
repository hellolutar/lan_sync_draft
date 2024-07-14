#ifndef __NET_FRAMEWORK_H_
#define __NET_FRAMEWORK_H_

#include <memory>

#include "framework/itf/net/transport_proto.h"

class NetframeworkEngine
{
public:
    NetframeworkEngine() {}
    virtual ~NetframeworkEngine() {}
    virtual std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) = 0;
    virtual std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) = 0;
    virtual std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &addr) = 0;
    virtual std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &addr) = 0;

    virtual void unregisterUdpCli(const NetAddr &addr) = 0;

    virtual std::shared_ptr<TcpCli> findTcpCli(const NetAddr &addr) = 0;
};

#endif