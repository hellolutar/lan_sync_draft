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

void ProtoSession::write(uint8_t *data, uint64_t size)
{
    static_cast<const ProtoSession &>(*this).write(data, size);
}

void ProtoSession::write(uint8_t *data, uint64_t size) const
{
    if (tcli_ != nullptr)
    {
        tcli_->write(data, size);
        return;
    }
    throw NotFoundException("the tcli_ is nullptr!");
}

void ProtoSession::bind(const NetAddr &from, const LanSyncPkt &pkt)
{
    if (tcli_ != nullptr)
        throw NotFoundException("the tcli_ has been bound!");

    NetAddr peer(from);
    peer.setType(TransportType::TCP);

    auto eg = Netframework::getEngine();
    tcli_ = eg->findTcpCli(peer);
    tcli_->bind(core_logic_);
    state_ = SessionState::ESTABLISED;
}

void ProtoSession::call(const NetAddr &from, const LanSyncPkt &pkt)
{
    if (tcli_ != nullptr)
        throw NotFoundException("the tcli_ has already been created!");

    uint16_t peerPort = *(reinterpret_cast<uint16_t*>(pkt.getPayload()));
    NetAddr peer(from);
    peer.setType(TransportType::TCP);
    peer.setPort(peerPort);

    auto eg = Netframework::getEngine();
    state_ = SessionState::CONNECTING;
    tcli_ = eg->connectWithTcp(peer);
    tcli_->bind(core_logic_);

    reply_hello_ack();
    state_ = SessionState::ESTABLISED;
}

void ProtoSession::handleDisconnect(LanSyncPkt &pkt, const NetAddr &from)
{
    state_ = SessionState::CONNECTING;

    if (pkt.getPayloadSize() == 0)
        return;

    uint16_t peer_tcp_port = *(reinterpret_cast<uint16_t *>(pkt.getPayload()));

    NetAddr peer(from);
    peer.setPort(peer_tcp_port);

    auto eg = Netframework::getEngine();
    tcli_ = eg->connectWithTcp(from);
    tcli_->bind(core_logic_);

    reply_hello_ack();
}

void ProtoSession::reply_hello_ack()
{
    LanSyncPkt replyPkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK);
    BufBaseonEvent buf;
    replyPkt.writeTo(buf);
    tcli_->write(buf.data().get(), buf.size());
}
