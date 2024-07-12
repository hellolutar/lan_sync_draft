#include "endpoint/setup_road_test.h"
#include <cstring>

LanSyncPkt pkt_hello(void *payload = nullptr, uint64_t size = 0)
{
    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO);
    if (payload != nullptr)
        pkt.setPayload(payload, size);
    return pkt;
}

LanSyncPkt pkt_hello_ack(void *payload = nullptr, uint64_t size = 0)
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

/**
 * 1. me.udp_server <- hello
 * 2. me.tcp_cli -> hello_ack
 */
TEST_F(EndPointTestCaseSimple, server_recv_hello_then_reply_helloack)
{
    auto peer_udp_cli_addr = NetAddr("0.0.0.1:18080", TransportType::UDP);
    auto peer_udp_srv_addr = NetAddr("0.0.0.1:8080", TransportType::UDP);
    auto peer_tcp_cli_addr = NetAddr("0.0.0.1:18080", TransportType::TCP);
    auto peer_tcp_srv_addr = NetAddr("0.0.0.1:8080", TransportType::TCP);

    uint16_t port = 8080;
    my_udp_srv_receive_from(peer_udp_cli_addr, pkt_hello(&port, sizeof(uint16_t)));
    assert_my_tcp_cli_sended_to(peer_tcp_srv_addr, pkt_hello_ack());
}

/**
 * 1. me.udp_cli -> hello
 */
TEST_F(EndPointTestCaseSimple, trigger_send_hello_with_udp)
{
    auto peer_udp_cli_addr = NetAddr("0.0.0.1:18080", TransportType::UDP);
    auto peer_udp_srv_addr = NetAddr("0.0.0.1:8080", TransportType::UDP);
    auto peer_tcp_cli_addr = NetAddr("0.0.0.1:18080", TransportType::TCP);
    auto peer_tcp_srv_addr = NetAddr("0.0.0.1:8080", TransportType::TCP);

    teg_->tick(2000);

    uint16_t port = 8080;
    assert_my_udp_cli_sended_to(peer_udp_srv_addr, pkt_hello(&port, sizeof(uint16_t)));
}

/**
 * 1. me.udp_server <- hello     <-  peer.udp_cli
 * 2. me.tcp_cli    -> hello_ack ->  peer.tcp_server
 * 3. me.tcp_cli    <- req_idx   <-  peer.tcp_server
 * 4. me.tcp_cli    -> rs_table  ->  peer.tcp_server
 */
TEST_F(EndPointTestCaseSimple, communication_for_req_and_reply_idx_base_server)
{
    auto peer_udp_cli_addr = NetAddr("0.0.0.1:18080", TransportType::UDP);
    auto peer_udp_srv_addr = NetAddr("0.0.0.1:8080", TransportType::UDP);
    auto peer_tcp_cli_addr = NetAddr("0.0.0.1:18080", TransportType::TCP);
    auto peer_tcp_srv_addr = NetAddr("0.0.0.1:8080", TransportType::TCP);

    uint16_t port = 8080;
    my_udp_srv_receive_from(peer_udp_cli_addr, pkt_hello(&port, sizeof(uint16_t)));
    assert_my_tcp_cli_sended_to(peer_tcp_srv_addr, pkt_hello_ack());

    vector<Resource> tb = {{"java", "java", "java.lang.string", "path", 18}, {"C++", "C++", "since 1985", "path", 1985}};
    rm_->setIdx(tb);
    rm_->setNeedToSync(tb);

    // 对端请求索引
    my_tcp_cli_receive_from(peer_tcp_srv_addr, pkt_req_idx());

    assert_my_tcp_cli_sended_to(peer_tcp_srv_addr, pkt_reply_idx(tb));
}