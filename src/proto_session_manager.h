#ifndef __PROTO_SESSION_MANAGER_H_
#define __PROTO_SESSION_MANAGER_H_

#include <atomic>
#include <memory>

#include "road_maintainer.h"
#include "framework/net/net_framework.h"
#include "proto/lan_share_protocol.h"

class ProtoSessionManagerLogic : public Logic
{
private:
    std::uint16_t session_id_ = 0;
    std::atomic<uint16_t> incr_{0};

    std::shared_ptr<std::vector<std::shared_ptr<RoadMaintainer>>> roads_;

    const uint16_t genRoadId();

public:
    ProtoSessionManagerLogic(){};
    ProtoSessionManagerLogic(std::uint16_t s_id, std::shared_ptr<std::vector<std::shared_ptr<RoadMaintainer>>> &rs)
        : session_id_(s_id), roads_(rs) {}
    ~ProtoSessionManagerLogic() {}

    const uint64_t isExtraAllDataNow(uint8_t *data, uint64_t data_len) const override;

    /**
     * 由TcpServer、UdpServer、TcpCli、UdpCli等调用
     */
    void recv(const NetAddr &from, uint8_t *data, uint64_t size) override;
};

class ProtoSessionManager : public std::enable_shared_from_this<ProtoSessionManager>
{
private:
    std::uint16_t id_;
    std::shared_ptr<TcpServer> tcpser_;
    std::shared_ptr<UdpServer> udpser_;
    std::vector<std::shared_ptr<RoadMaintainer>> roads_;
    std::shared_ptr<ProtoSessionManagerLogic> logic_;
    logicRecvCB logic_recv_f_;

    std::shared_ptr<RoadMaintainer> findRoad(uint32_t road_id) const;

public:
    ProtoSessionManager(logicRecvCB logic_recv_f, NetAddr port) : logic_recv_f_(logic_recv_f)
    {
        auto net = Netframework::getEngine();
        auto v = std::make_shared<std::vector<std::shared_ptr<RoadMaintainer>>>(roads_);
        logic_ = std::make_shared<ProtoSessionManagerLogic>(id_, v);

        tcpser_ = net->addTcpServer(port);
        udpser_ = net->addUdpServer(port);

        tcpser_->bind(logic_);
        udpser_->bind(logic_);
    }
    ProtoSessionManager(ProtoSessionManager &&m)
        : logic_recv_f_(m.logic_recv_f_), tcpser_(m.tcpser_), udpser_(m.udpser_)
    {
        m.tcpser_ = nullptr;
        m.udpser_ = nullptr;
    };

    ~ProtoSessionManager()
    {
        roads_.clear();
        tcpser_ = nullptr;
        udpser_ = nullptr;
    }

    void write(uint32_t road_id, uint8_t *data, uint64_t size) const;

    const uint16_t getId() const;
};

#endif