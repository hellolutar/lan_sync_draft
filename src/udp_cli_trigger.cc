#include "udp_cli_trigger.h"

void UdpCliTrigger::trigger()
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO);
    uint16_t port = 8080;
    pkt.setPayload(&port, sizeof(uint16_t));
    BufBaseonEvent buf;
    pkt.writeTo(buf);
    ctx_.write(buf.data(), buf.size());
}

const bool UdpCliTrigger::isRunning() const
{
    return running_;
}

std::uint16_t UdpCliTrigger::getId() const
{
    return id_;
}
