#include "road_maintainer.h"

void RoadMaintainerLogic::recv(const NetAddr &from, uint8_t *data, uint64_t size)
{
    LanSyncPkt pkt(data);

    auto st = state_;
    switch (st)
    {
    case RoadState::DISCONNECT:
        handleDisconnect(pkt, from);
        break;
    case RoadState::ESTABLISED:
        // logic_recv_f_(id_, data, size);
        break;
    case RoadState::DISRUPTION:
        // if (trg_.isRunning())
        //     del_trg_f_(trg_.getId());
        break;
    case RoadState::CONNECTING:
    default:
        break;
    }
}

void RoadMaintainerLogic::handleDisconnect(LanSyncPkt &pkt, const NetAddr &from)
{
    // if (trg_.isRunning())
    // TriggerDispatch.unregister(trg_);

    state_ = RoadState::CONNECTING;

    if (pkt.getPayloadSize() == 0)
        return;

    uint16_t peer_tcp_port = *(reinterpret_cast<uint16_t *>(pkt.getPayload()));

    auto eg = Netframework::getEngine();
    NetAddr peer(from);
    peer.setPort(peer_tcp_port);

    auto cli = new_tcpcli_f_(peer);

    LanSyncPkt replyPkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK);
    BufBaseonEvent buf;
    replyPkt.writeTo(buf);
    cli->write(reinterpret_cast<uint8_t *>(buf.data()), buf.size());
}

const uint64_t RoadMaintainerLogic::isExtraAllDataNow(uint8_t *data, uint64_t data_len) const
{
    return 0;
}

void RoadMaintainer::tcp_write(uint8_t *data, uint64_t size)
{
    cli_->write(data, size);
}

std::shared_ptr<TcpCli> RoadMaintainer::new_tcp_cli(NetAddr peer)
{
    auto eg = Netframework::getEngine();
    cli_ = eg->connectWithTcp(peer);
    cli_->bind(std::shared_ptr<Logic>(logic_));
    return cli_;
}

void RoadMaintainer::udp_write(const NetAddr &peer, uint8_t *data, uint64_t size)
{
    UdpCli cli(peer);
    cli.bind(logic_);
    cli.write(data, size);
}

bool RoadMaintainer::isMineRoad(const NetAddr &from)
{
    // if (trg_.isRunning())
    // {
    //     auto peer = trg_.getUdpCli().peer();
    //     if (peer == from)
    //         return true;
    // }

    return cli_->peer() == from;
}

const std::uint16_t RoadMaintainer::getId() const
{
    return id_;
}

void RoadMaintainer::recv(const NetAddr &peer, uint8_t *data, uint64_t size)
{
    logic_->recv(peer, data, size);
}

void RoadMaintainer::new_trigger(const NetAddr &peer)
{
    UdpCliTrigger trg;
}
