#include "dep/setup_road_test.h"

/**
 * 1. me.udp_cli -> hello
 */
TEST_F(EndPointTestCaseSimple, trigger_send_hello_with_udp)
{
    teg_->tick(2000);

    uint16_t port = 8080;
    assert_my_udp_cli_sended_to(peer_udp_srv_addr, pkt_hello(&port, sizeof(uint16_t)));
}

/**
 * 1. me.udp_cli       -> hello     ->  peer.udp_server
 * 2. me.tcp_server    <- hello_ack <-  peer.tcp_cli
 * 3. me.tcp_server    -> req_idx   ->  peer.tcp_cli
 * 4. me.tcp_server    <- rply_idx  <-  peer.tcp_cli
 * 5. me.tcp_server    -> get_rs    ->  peer.tcp_cli
 * 6. me.tcp_server    <- reply_rs  <-  peer.tcp_cli
 */
TEST_F(EndPointTestCaseSimple, me_tcp_server_get_rs)
{
    teg_->tick(2000);

    uint16_t port = 8080;
    assert_my_udp_cli_sended_to(peer_udp_srv_addr, pkt_hello(&port, sizeof(uint16_t)));

    neg_->connectWithTcp(peer_tcp_cli_addr); // register peer tcpcli to netframework, that we can found it in follow code.

    my_tcp_srv_receive_from(peer_tcp_cli_addr, pkt_hello_ack());
    assert_my_tcp_srv_sended_to(peer_tcp_cli_addr, pkt_req_idx());

    vector<Resource> tb = {{"java", "uri.java", "java.lang.string", "path", 18}, {"C++", "uri.C++", "since 1985", "path", 10 * BLOCK_SIZE}};
    rm_->setIdx(tb);
    rm_->setNeedToSync(tb);

    my_tcp_srv_receive_from(peer_tcp_cli_addr, pkt_reply_idx(tb));

    assert_my_tcp_srv_sended_getrs_to(peer_tcp_cli_addr);

    my_tcp_srv_receive_reply_rs_from(peer_tcp_cli_addr);

    ASSERT_GT(tm_->getSuccessTasks().size(), 0);

    uint8_t limit = 100;
    uint8_t i = 0;
    while (tm_->getSuccessTasks().size() != tm_->getAllTasks().size())
    {
        teg_->tick(2000);
        my_tcp_srv_receive_reply_rs_from(peer_tcp_cli_addr);
        ASSERT_LT(i++, limit);
    }
}
