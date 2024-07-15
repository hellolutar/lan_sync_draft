#include "dep/setup_road_test.h"

LanSyncPkt EndPointTestCaseSimple::pkt_hello(void *payload, uint64_t size)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO);
    if (payload != nullptr)
        pkt.setPayload(payload, size);
    return pkt;
}

LanSyncPkt EndPointTestCaseSimple::pkt_hello_ack(void *payload, uint64_t size)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK);
    if (payload != nullptr)
        pkt.setPayload(payload, size);
    return pkt;
}

LanSyncPkt EndPointTestCaseSimple::pkt_req_idx()
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_GET_TABLE_INDEX);
    return pkt;
}

LanSyncPkt EndPointTestCaseSimple::pkt_reply_idx(vector<Resource> tb)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_REPLY_TABLE_INDEX);
    auto dto = ResourceSerializer::serialize(tb);
    pkt.setPayload(dto.data.get(), dto.size);
    return pkt;
}

LanSyncPkt EndPointTestCaseSimple::pkt_req_rs(Resource r)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, LAN_SYNC_TYPE_GET_RESOURCE);

    string range_hdr = Range(0, r.getSize()).to_string();
    pkt.addXheader(XHEADER_URI, r.getUri());
    pkt.addXheader(XHEADER_RANGE, range_hdr);
    return pkt;
}