#ifndef __LANSYNC_PROTO_SERVER_H_
#define __LANSYNC_PROTO_SERVER_H_

#include <memory>
#include <vector>

#include "logic/logic.h"
#include "proto_session.h"

class ProtoServerRecv : public Logic
{
protected:
    std::shared_ptr<std::vector<std::shared_ptr<ProtoSession>>> sess_;
    std::shared_ptr<Logic> core_logic_;

public:
    ProtoServerRecv(std::shared_ptr<Logic> logic)
        : core_logic_(logic) ,
        sess_(std::make_shared<std::vector<std::shared_ptr<ProtoSession>>>())
        {}
    ~ProtoServerRecv() {}

    /**
     * 由TcpServer、UdpServer 调用； tcpcli 直接调用的core_logic_
     */
    void recv(const NetAddr &from, uint8_t *data, uint64_t size) override;
    const uint64_t isExtraAllDataNow(uint8_t *data, uint64_t data_len) const override
    {
        return core_logic_->isExtraAllDataNow(data, data_len);
    }
};

class ProtoServer
{
private:
    std::shared_ptr<TcpServer> tcpser_;
    std::shared_ptr<UdpServer> udpser_;

    std::shared_ptr<Logic> core_logic_;
    std::shared_ptr<ProtoServerRecv> server_logic_;
    std::shared_ptr<std::vector<ProtoSession>> sess_;

public:
    ProtoServer(NetAddr port, std::shared_ptr<Logic> logic)
        : core_logic_(logic),
          server_logic_(std::make_shared<ProtoServerRecv>(logic))
    {
        auto net = Netframework::getEngine();

        tcpser_ = net->addTcpServer(port);
        udpser_ = net->addUdpServer(port);

        tcpser_->bind(server_logic_);
        udpser_->bind(server_logic_);
    }
    ~ProtoServer()
    {
        tcpser_ = nullptr;
        udpser_ = nullptr;
        core_logic_ = nullptr;
        server_logic_ = nullptr;
        sess_->clear();
        sess_ = nullptr;
    }

    const ProtoSession &findSession(const NetAddr &peer);
};

#endif