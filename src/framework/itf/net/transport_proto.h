#ifndef __TRANSPORT_PROTO_H_
#define __TRANSPORT_PROTO_H_

#include <memory>

#include "logic/logic.h"
#include "framework/dep/outputstream.h"

class NetAbility
{
protected:
    std::shared_ptr<LogicProto> logic_ = nullptr;

public:
    NetAbility() {};
    virtual ~NetAbility()
    {
        logic_ = nullptr;
    };
    virtual const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t size);
    virtual void recv(NetAbilityContext &ctx, std::shared_ptr<uint8_t[]> data, uint64_t size);

    void bind(std::shared_ptr<LogicProto> logic);
};

class ConnCli : public NetAbility
{
protected:
    NetAddr peer_;
    std::shared_ptr<OutputStream> os_ = nullptr;

public:
    ConnCli(/* args */) : peer_() {}
    ConnCli(NetAddr peer) : peer_(peer) {};
    virtual ~ConnCli() {}

    const NetAddr &peer() const;

    virtual void write(std::shared_ptr<uint8_t[]> data, uint64_t size);
    void setOutputStream(std::shared_ptr<OutputStream> os); // 由engine调用
    std::shared_ptr<OutputStream> os();
};

class TcpCli : public ConnCli
{
public:
    TcpCli(NetAddr peer) : ConnCli(peer) {}
    virtual ~TcpCli();
};

class UdpCli : public ConnCli
{

private:
    int sock_;

public:
    UdpCli() {};
    UdpCli(int sock, NetAddr peer) : sock_(sock), ConnCli(peer) {}
    virtual ~UdpCli();
};

class TcpServer : public NetAbility
{
protected:
    NetAddr port_;

public:
    TcpServer() {}
    TcpServer(NetAddr port) : port_(port) {}
    virtual ~TcpServer();

    virtual const std::shared_ptr<LogicProto> getLogic() const;
};

class UdpServer : public NetAbility
{
protected:
    NetAddr port_;

public:
    UdpServer() {}
    UdpServer(NetAddr port) : port_(port) {}
    virtual ~UdpServer();
};

#endif