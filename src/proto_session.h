#ifndef __PROTO_SESSION_MANAGER_H_
#define __PROTO_SESSION_MANAGER_H_

#include <atomic>

#include "buf/buf_base_on_event.h"
#include "framework/itf/net/net_framework.h"
#include "proto/lan_share_protocol.h"
#include "exc/not_found_exc.h"

enum class SessionState
{
    DISCONNECT,
    CONNECTING,
    ESTABLISED,
    DISRUPTION
};

class ProtoSession
{
protected:
    std::uint32_t id_;
    SessionState state_ = SessionState::DISCONNECT;
    std::shared_ptr<TcpCli> tcli_;

    std::shared_ptr<LogicWrite> core_logic_;

    void handleDisconnect(LanSyncPkt &pkt, const NetAddr &from);

    void reply_hello_ack();

public:
    ProtoSession(std::shared_ptr<LogicWrite> logic)
        : core_logic_(logic)
    {
    }
    ProtoSession(ProtoSession &&m){};

    virtual ~ProtoSession();

    const uint16_t getId() const;

    bool isMe(const NetAddr &peer);

    void recv(NetAbilityContext &ctx, std::shared_ptr<uint8_t[]> data, uint64_t size);
    void write(std::shared_ptr<uint8_t[]> data, uint64_t size);
    void write(std::shared_ptr<uint8_t[]> data, uint64_t size) const;

    bool bind(const NetAddr &peer, const LanSyncPkt & pkt);
    bool call(const NetAddr &peer, const LanSyncPkt & pkt);
};

#endif