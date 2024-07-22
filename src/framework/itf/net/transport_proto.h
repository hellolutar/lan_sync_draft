#ifndef __TRANSPORT_PROTO_H_
#define __TRANSPORT_PROTO_H_

#include <memory>

#include "logic/logic.h"
#include "framework/dep/outputstream.h"

class NetAbility
{
protected:
    std::unique_ptr<OutputStream> os_ = nullptr;
    std::shared_ptr<LogicWrite> logic_ = nullptr;

public:
    NetAbility(){};
    virtual ~NetAbility()
    {
        os_ = nullptr;
        logic_ = nullptr;
    };
    virtual const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t size);
    virtual void recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size);
    virtual void write(std::shared_ptr<uint8_t[]> data, uint64_t size);

    void setOutputStream(std::unique_ptr<OutputStream> &&os); // 由engine调用
    void bind(std::shared_ptr<LogicWrite> logic);

    const std::unique_ptr<OutputStream> &getOutputStream();
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
public:
    TcpCli(NetAddr peer) : ConnCli(peer) {}
    ~TcpCli();
};

class UdpCli : public ConnCli
{

private:
    int sock_;

public:
    UdpCli(){};
    UdpCli(int sock, NetAddr peer) : sock_(sock), ConnCli(peer) {}
    virtual ~UdpCli();

    virtual void write(std::shared_ptr<uint8_t[]> data, uint64_t size) override;
};

class TcpServer : public NetAbility
{
private:
    NetAddr port_;

public:
    TcpServer() {}
    TcpServer(NetAddr port) : port_(port) {}
    ~TcpServer();

    const std::shared_ptr<LogicWrite> getLogic() const;
};

class UdpServer : public NetAbility
{
private:
    NetAddr port_;

public:
    UdpServer() {}
    UdpServer(NetAddr port) : port_(port) {}
    ~UdpServer();
};

#endif