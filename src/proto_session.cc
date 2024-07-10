#include "proto_session.h"

static std::atomic<uint16_t> incr_{0};

const uint16_t genSeesionId(uint16_t server_id)
{
    // todo 目前的用法不正确
    incr_.store(incr_.load() + 1);
    return server_id << 16 | incr_.load();
}

const uint16_t ProtoSession::getId() const
{
    return id_;
}

bool ProtoSession::isMe(const NetAddr &peer)
{
    return tcli_->peer() == peer;
}

void ProtoSession::recv(const NetAddr &peer, uint8_t *data, uint64_t size)
{
    LanSyncPkt pkt(data);

    auto st = state_;
    switch (st)
    {
    case SessionState::DISCONNECT:
        handleDisconnect(pkt, peer);
        break;
    case SessionState::ESTABLISED:
        core_logic_->recv(peer, data, size);
        break;
    case SessionState::DISRUPTION:
        // if (trg_.isRunning())
        //     del_trg_f_(trg_.getId());
        break;
    case SessionState::CONNECTING:
    default:
        break;
    }
}

void ProtoSession::write(const NetAddr &peer, uint8_t *data, uint64_t size)
{
    if (tcli_ != nullptr)
        tcli_->write(data, size);
    throw "the tcli_ is nullptr!";
}

void ProtoSession::bind(const NetAddr &peer)
{
    if (tcli_ != nullptr)
        throw "the tcli_ has been bound!";

    auto eg = Netframework::getEngine();
    tcli_ = eg->findTcpCli(peer);
    tcli_->bind(core_logic_);
}

void ProtoSession::call(const NetAddr &peer)
{
    if (tcli_ != nullptr)
        throw "the tcli_ has already been created!";

    auto eg = Netframework::getEngine();
    tcli_ = eg->connectWithTcp(peer);
    tcli_->bind(core_logic_);
}

void ProtoSession::handleDisconnect(LanSyncPkt &pkt, const NetAddr &from)
{
    state_ = SessionState::CONNECTING;

    if (pkt.getPayloadSize() == 0)
        return;

    uint16_t peer_tcp_port = *(reinterpret_cast<uint16_t *>(pkt.getPayload()));

    auto eg = Netframework::getEngine();
    NetAddr peer(from);
    peer.setPort(peer_tcp_port);

    auto eg = Netframework::getEngine();
    tcli_ = eg->connectWithTcp(from);
    tcli_->bind(core_logic_);

    LanSyncPkt replyPkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK);
    BufBaseonEvent buf;
    replyPkt.writeTo(buf);
    tcli_->write(reinterpret_cast<uint8_t *>(buf.data()), buf.size());
}