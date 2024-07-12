#include "logic_core.h"

void LogicCore::helloAck(const NetAddr &peer, const LanSyncPkt &pkt)
{
    auto idx = rm_->idx();
    auto dto = ResourceSerializer::serialize(idx);
    LanSyncPkt p = {lan_sync_version::VER_0_1, LAN_SYNC_TYPE_GET_TABLE_INDEX};

    BufBaseonEvent buf;
    p.writeTo(buf);
    adapter_->write(peer, buf.data().get(), buf.size());
    // todo 这里存在虚假指针问题，因为outputstream是放入缓冲区，放完以后，执行到此，执行到
    // 括号外，buf被释放，会导致buf.data被释放。
}

void LogicCore::reqIdx(const NetAddr &peer, const LanSyncPkt &pkt)
{
    auto idx = rm_->idx();
    auto dto = ResourceSerializer::serialize(idx);
    LanSyncPkt p = {lan_sync_version::VER_0_1, LAN_SYNC_TYPE_REPLY_TABLE_INDEX};

    p.setPayload(dto.data.get(), dto.size);

    BufBaseonEvent buf;
    p.writeTo(buf);
    adapter_->write(peer, buf.data().get(), buf.size());
    // todo 这里存在虚假指针问题，因为outputstream是放入缓冲区，放完以后，执行到此，执行到
    // 括号外，buf被释放，会导致buf.data被释放。
}

void LogicCore::reqRs(const NetAddr &peer, const LanSyncPkt &pkt)
{
}

void LogicCore::recvIdx(const NetAddr &peer, const LanSyncPkt &pkt)
{
    // todo
    std::vector<Resource> tb = ResourceSerializer::deserialize(reinterpret_cast<uint8_t *>(pkt.getPayload()), pkt.getPayloadSize());
    auto need_to_sync_rs = rm_->need_to_sync(tb);

    // auto dto = ResourceSerializer::serialize(need_to_sync_rs);
    LanSyncPkt p = {lan_sync_version::VER_0_1, LAN_SYNC_TYPE_GET_RESOURCE};

    // todo syn rs by content-range

    BufBaseonEvent buf;
    p.writeTo(buf);
    adapter_->write(peer, buf.data().get(), buf.size());
    // todo 这里存在虚假指针问题，因为outputstream是放入缓冲区，放完以后，执行到此，执行到
    // 括号外，buf被释放，会导致buf.data被释放。
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
    case lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK:
        helloAck(peer, pkt);
        break;
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

void LogicCore::setResourceManager(std::shared_ptr<ResourceManager> rm)
{
    rm_ = rm;
}
