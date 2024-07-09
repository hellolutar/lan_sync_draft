#ifndef __ROAD_MAINTAINER_H_
#define __ROAD_MAINTAINER_H_

#include <cstdint>
#include <vector>
#include <functional>
#include <chrono>

#include "udp_cli_trigger.h"
#include "proto/lan_share_protocol.h"
#include "framework/net/net_framework.h"
#include "buf/buf_base_on_event.h"

using logicRecvCB = std::function<void(uint32_t, uint8_t *, uint64_t)>;
using sessionRecvCB = std::function<void(const NetAddr &, uint8_t *, uint64_t)>;
using newTcpCliCB = std::function<std::shared_ptr<TcpCli>(NetAddr addr)>;

enum class RoadState
{
    DISCONNECT,
    CONNECTING,
    ESTABLISED,
    DISRUPTION
};

class RoadMaintainerLogic : public Logic
{
private:
    RoadState state_ = RoadState::DISCONNECT;
    std::uint32_t road_id_ = 0;

    logicRecvCB logic_recv_f_;
    sessionRecvCB session_recv_f_;
    newTcpCliCB new_tcpcli_f_;
    std::function<void(uint16_t)> del_trg_f_;

public:
    RoadMaintainerLogic(/* args */) {}
    RoadMaintainerLogic(std::uint32_t rid, logicRecvCB logic_f, sessionRecvCB session_f, newTcpCliCB new_tcpcli_f)
        : road_id_(rid), logic_recv_f_(logic_f), session_recv_f_(session_f), new_tcpcli_f_(new_tcpcli_f) {}
    ~RoadMaintainerLogic() {}

    void recv(const NetAddr &peer, uint8_t *data, uint64_t size) override;
    const uint64_t isExtraAllDataNow(uint8_t *data, uint64_t data_len) const override;

    void handleDisconnect(LanSyncPkt &pkt, const NetAddr &from);
};

class RoadMaintainer
{
private:
    std::uint32_t id_ = 0; // session_id, road_id

    std::shared_ptr<TcpCli> t_cli_;

    std::shared_ptr<RoadMaintainerLogic> logic_;

public:
    RoadMaintainer(uint32_t id, logicRecvCB logic_f, sessionRecvCB session_f) : id_(id)
    {

        auto f = std::bind(&RoadMaintainer::new_tcp_cli, this, std::placeholders::_1);
        logic_ = std::make_shared<RoadMaintainerLogic>(id, logic_f, session_f, f);
    }
    ~RoadMaintainer() {}

    void tcp_write(uint8_t *data, uint64_t size);

    std::shared_ptr<TcpCli> new_tcp_cli(NetAddr addr);

    void udp_write(const NetAddr &peer, uint8_t *data, uint64_t size);

    bool isMineRoad(const NetAddr &from);

    const std::uint16_t getId() const;

    void recv(const NetAddr &peer, uint8_t *data, uint64_t size);

    void new_trigger(const NetAddr &peer);
};

#endif