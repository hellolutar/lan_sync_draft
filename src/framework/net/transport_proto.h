#ifndef __TRANSPORT_PROTO_H_
#define __TRANSPORT_PROTO_H_

#include <memory>

#include "framework/net/net_addr.h"
#include "logic/logic.h"

class OutputStream
{
private:
    bool closed_ = true;

public:
    OutputStream() {}
    OutputStream(bool closed) : closed_(closed) {}
    virtual ~OutputStream() {}

    virtual void close();
    virtual bool isClosed();
    virtual void write(uint8_t *data, const uint64_t size) = 0;
};

class NetAbility
{
private:
    std::shared_ptr<OutputStream> os_ = nullptr;
    std::shared_ptr<Logic> logic_ = nullptr;

public:
    NetAbility(){};
    virtual ~NetAbility()
    {
        os_ = nullptr;
        logic_ = nullptr;
    };
    virtual const uint64_t isExtraAllDataNow(uint8_t *data, uint64_t size);
    virtual void recv(const NetAddr &peer, uint8_t *data, uint64_t size);
    virtual void write(uint8_t *data, uint64_t size);

    void setOutputStream(std::shared_ptr<OutputStream> os);
    void bind(std::shared_ptr<Logic> logic);

    std::shared_ptr<OutputStream> getOutputStream();
};

class ConnCli : public NetAbility
{
protected:
    NetAddr peer_;

public:
    ConnCli(/* args */) {}
    ConnCli(NetAddr peer) : peer_(peer){};
    virtual ~ConnCli() {}

    const NetAddr &peer() const;
};

class TcpCli : public ConnCli
{
private:
    std::shared_ptr<Logic> logic_;

public:
    TcpCli(NetAddr peer) : ConnCli(peer) {}
    ~TcpCli() {}
};

class UdpCli : public ConnCli
{

public:
    UdpCli(){};
    UdpCli(NetAddr peer) : ConnCli(peer) {}
    ~UdpCli() {}
};

class TcpServer : public NetAbility
{
private:
    NetAddr port_;

public:
    TcpServer() {}
    TcpServer(NetAddr port) : port_(port) {}
    ~TcpServer() {}
};

class UdpServer : public NetAbility
{
private:
    NetAddr port_;

public:
    UdpServer() {}
    UdpServer(NetAddr port) : port_(port) {}
    ~UdpServer() {}
};

#endif