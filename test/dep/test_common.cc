
#include "dep/test_common.h"

std::string dic = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

std::string genStr(uint32_t size)
{
    std::stringstream ss{};
    for (uint64_t i = 0; i < size;)
    {
        uint64_t subsize = std::min(static_cast<uint64_t>(size - i), dic.size());
        auto tmp = dic.substr(0, subsize);
        ss << tmp;
        i += subsize;
    }
    return ss.str();
}

std::shared_ptr<uint8_t[]> gen_u8_array(uint64_t size)
{
    std::shared_ptr<uint8_t[]> ret(new uint8_t[size]());
    auto str = genStr(size);
    memcpy(ret.get(), str.data(), size);
    return ret;
}

LanSyncPkt pkt_hello(void *payload, uint64_t size)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO);
    if (payload != nullptr)
        pkt.setPayload(payload, size);
    return pkt;
}

LanSyncPkt pkt_hello_ack(void *payload, uint64_t size)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK);
    if (payload != nullptr)
        pkt.setPayload(payload, size);
    return pkt;
}

LanSyncPkt pkt_req_idx()
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_GET_TABLE_INDEX);
    return pkt;
}

LanSyncPkt pkt_reply_idx(vector<Resource> tb)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_REPLY_TABLE_INDEX);
    auto dto = ResourceSerializer::serialize(tb);
    pkt.setPayload(dto.data.get(), dto.size);
    return pkt;
}

LanSyncPkt pkt_req_rs(Resource r)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, LAN_SYNC_TYPE_GET_RESOURCE);

    string range_hdr = Range(0, r.getSize()).to_string();
    pkt.addXheader(XHEADER_URI, r.getUri());
    pkt.addXheader(XHEADER_RANGE, range_hdr);
    return pkt;
}