#ifndef __PROTO_SESSION_MANAGER_H_
#define __PROTO_SESSION_MANAGER_H_

#include <atomic>
#include <memory>

#include "buf/buf_base_on_event.h"
#include "framework/net/net_framework.h"
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
private:
    std::uint32_t id_;
    SessionState state_ = SessionState::DISCONNECT;
    std::shared_ptr<TcpCli> tcli_;

    std::shared_ptr<Logic> core_logic_;

    void handleDisconnect(LanSyncPkt &pkt, const NetAddr &from);

    void reply_hello_ack();

public:
    ProtoSession(std::shared_ptr<Logic> logic)
        : core_logic_(logic)
    {
    }
    ProtoSession(ProtoSession &&m){};

    ~ProtoSession()
    {
        tcli_ = nullptr;
        core_logic_ = nullptr;
    }

    const uint16_t getId() const;

    bool isMe(const NetAddr &peer);

    void recv(const NetAddr &peer, uint8_t *data, uint64_t size);
    void write(uint8_t *data, uint64_t size);
    void write(uint8_t *data, uint64_t size) const;

    void bind(const NetAddr &peer);
    void call(const NetAddr &peer);
};

#endif