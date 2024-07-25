#include "udp_cli_trigger.h"

void UdpCliTrigger::trigger()
{
    for (auto &&ctx : ctxs_)
    {
        LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO);
        uint16_t port = default_tcp_srv_port;
        pkt.setPayload(&port, sizeof(uint16_t));
        BufBaseonEvent buf;
        pkt.writeTo(buf);
        ctx.write(buf.data(), buf.size());
    }
}

const bool UdpCliTrigger::isRunning() const
{
    return running_;
}

std::uint16_t UdpCliTrigger::getId() const
{
    return id_;
}

void UdpCliTrigger::addCtx(const NetworkContext &ctx)
{
    ctxs_.push_back(ctx);
}

void UdpCliTrigger::delCtx(const NetAddr &addr)
{
    if (ctxs_.size() == 0)
        return;

    for (auto iter = ctxs_.end() - 1; iter >= ctxs_.begin(); iter--)
    {
        auto c = *iter;
        if (c.getPeer().str() == addr.str()){
            ctxs_.erase(iter);
            return;
        }
    }
}

uint64_t UdpCliTrigger::ctxSize()
{
    return ctxs_.size();
}
