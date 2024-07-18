#ifndef __NET_FRAMEWORK_H_
#define __NET_FRAMEWORK_H_

#include "framework/itf/net/transport_proto.h"

class Connection
{
protected:
    NetAddr addr_;

public:
    Connection(NetAddr a) : addr_(a) {}
    virtual ~Connection() {}

    bool isMe(const NetAddr &o)
    {
        return addr_.str() == o.str();
    }

    const NetAddr addr() const {
        return addr_;
    }
};

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

    virtual void addConn(std::shared_ptr<Connection>) = 0;

    virtual void start() = 0;
    virtual void shutdown() {};
};

#endif