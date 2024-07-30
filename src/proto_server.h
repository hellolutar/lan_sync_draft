#ifndef __LANSYNC_PROTO_SERVER_H_
#define __LANSYNC_PROTO_SERVER_H_

#include <vector>

#include "logic/logic.h"
#include "proto_session.h"
#include "framework/itf/net/transport_proto.h"

class ProtoServerRecv : public LogicWrite
{
protected:
    std::shared_ptr<std::vector<std::shared_ptr<ProtoSession>>> sess_;
    std::shared_ptr<LogicWrite> core_logic_;
    std::shared_ptr<ProtoSession> findSession(const NetAddr &peer);

public:
    ProtoServerRecv(std::shared_ptr<LogicWrite> logic, std::shared_ptr<std::vector<std::shared_ptr<ProtoSession>>> sess)
        : core_logic_(logic),
          sess_(sess)
    {
    }
    ~ProtoServerRecv() {}

    const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t data_len) const override;

    /**
     * 由TcpServer、UdpServer 调用； tcpcli 直接调用的core_logic_
     */
    void recv(NetAbilityContext &ctx, std::shared_ptr<uint8_t[]> data, uint64_t size) override;
};

class ProtoServer
{
private:
    std::shared_ptr<TcpServer> tcpser_;
    std::shared_ptr<UdpServer> udpser_;

    std::shared_ptr<LogicWrite> core_logic_;
    std::shared_ptr<ProtoServerRecv> server_logic_;
    std::shared_ptr<std::vector<std::shared_ptr<ProtoSession>>> sess_;

public:
    ProtoServer(NetAddr port, std::shared_ptr<LogicWrite> logic)
        : core_logic_(logic),
          sess_(std::make_shared<std::vector<std::shared_ptr<ProtoSession>>>())
    {
        server_logic_ = std::make_shared<ProtoServerRecv>(logic, sess_);

        auto net = Netframework::getEngine();

        NetAddr udp_port(port);
        NetAddr tcp_port(port);

        udp_port.setPort(default_udp_srv_port);
        tcp_port.setPort(default_tcp_srv_port);
        tcp_port.setType(TransportType::TCP);

        udpser_ = net->addUdpServer(udp_port);
        tcpser_ = net->addTcpServer(tcp_port);

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

    const std::optional<std::shared_ptr<ProtoSession>> findSession(const NetAddr &peer);

    bool rmSessionIfExist(const NetAddr &);
};

#endif