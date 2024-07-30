#ifndef __NET_FRAMEWORK_H_
#define __NET_FRAMEWORK_H_

#include <functional>

#include "framework/dep/connection.h"
#include "framework/itf/net/transport_proto.h"

class NetframeworkEngine
{
protected:
    std::function<void(const NetAddr &)> tcp_disconn_cb_ = nullptr;

public:
    NetframeworkEngine() {}
    virtual ~NetframeworkEngine() {}
    virtual std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) = 0;
    virtual std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) = 0;
    virtual std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &addr) = 0;
    virtual std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &addr) = 0;

    virtual std::shared_ptr<TcpCli> findTcpCli(const NetAddr &addr) = 0;

    virtual void addConn(std::shared_ptr<Connection>) = 0;

    virtual void start() = 0;
    virtual void shutdown() {};
    virtual void unRegister(const NetAddr &addr) {};

    virtual void setTcpDisConnCB(std::function<void(const NetAddr &)> cb)
    {
        tcp_disconn_cb_ = cb;
    };
};

#endif