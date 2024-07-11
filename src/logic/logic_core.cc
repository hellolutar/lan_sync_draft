#include "logic_core.h"

void LogicCore::reqIdx(const NetAddr &peer, const LanSyncPkt &pkt)
{
    auto idx = rm_.idx();
}

void LogicCore::reqRs(const NetAddr &peer, const LanSyncPkt &pkt)
{
}

void LogicCore::recvIdx(const NetAddr &peer, const LanSyncPkt &pkt)
{
}

void LogicCore::recvRs(const NetAddr &peer, const LanSyncPkt &pkt)
{
}

void LogicCore::shutdown(const NetAddr &peer, const LanSyncPkt &pkt)
{
}

const uint64_t LogicCore::isExtraAllDataNow(uint8_t *data, uint64_t data_len) const
{
    if (data_len < LEN_LAN_SYNC_HEADER_T)
        return 0;

    LanSyncPkt pkt(data);
    if (pkt.getTotalLen() > data_len)
        return 0;

    return pkt.getTotalLen();
}

void LogicCore::recv(const NetAddr &peer, uint8_t *data, uint64_t size)
{
    LanSyncPkt pkt(data);
    switch (pkt.getType())
    {
    case lan_sync_type_enum::LAN_SYNC_TYPE_GET_TABLE_INDEX:
        reqIdx(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_GET_RESOURCE:
        reqRs(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_REPLY_TABLE_INDEX:
        recvIdx(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_REPLY_RESOURCE:
        recvRs(peer, pkt);
        break;
    case lan_sync_type_enum::LAN_SYNC_TYPE_EXIT:
        shutdown(peer, pkt);
        break;
    default:
        throw NotFoundException("LogicCore::recv() can not match the pkt.type!");
        break;
    }
}

void LogicCore::setNetworkAdapter(std::shared_ptr<NetworkAdapter> ad)
{
    adapter_ = nullptr;
    adapter_ = ad;
}
