#include "dep/setup_road_test.h"

/**
 * 1. me.udp_server <- hello
 * 2. me.tcp_cli -> hello_ack
 */
TEST_F(EndPointTestCaseSimple, server_recv_hello_then_reply_helloack)
{
    uint16_t port = 8080;
    my_udp_srv_receive_from(peer_udp_cli_addr, pkt_hello(&port, sizeof(uint16_t)));
    assert_my_tcp_cli_sended_to(peer_tcp_srv_addr, pkt_hello_ack());
}

/**
 * 1. me.udp_server <- hello     <-  peer.udp_cli
 * 2. me.tcp_cli    -> hello_ack ->  peer.tcp_server
 * 3. me.tcp_cli    <- req_idx   <-  peer.tcp_server
 * 4. me.tcp_cli    -> rply_idx  ->  peer.tcp_server
 */
TEST_F(EndPointTestCaseSimple, communication_for_req__replyIdx_base_server)
{
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

/**
 * 1. me.udp_server    <- hello     <-  peer.udp_cli
 * 2. me.tcp_cli       -> hello_ack ->  peer.tcp_server
 * 3. me.tcp_cli       <- req_idx   <-  peer.tcp_server
 * 4. me.tcp_cli       -> rply_idx  ->  peer.tcp_server
 * 5. me.tcp_cli       <- get_rs    <-  peer.tcp_server
 * 6. me.tcp_cli       -> reply_rs  ->  peer.tcp_server
 */
TEST_F(EndPointTestCaseSimple, peer_tcp_server_get_rs)
{
    teg_->tick(2000);

    uint16_t port = 8080;
    my_udp_srv_receive_from(peer_udp_cli_addr, pkt_hello(&port, sizeof(uint16_t)));
    assert_my_tcp_cli_sended_to(peer_tcp_srv_addr, pkt_hello_ack());

    vector<Resource> tb = {{"java", "uri.java", "java.lang.string", "path", 18}, {"C++", "uri.C++", "since 1985", "path", 10 * BLOCK_SIZE}};
    rm_->setIdx(tb);
    rm_->setNeedToSync(tb);

    // 对端请求索引
    my_tcp_cli_receive_from(peer_tcp_srv_addr, pkt_req_idx());

    assert_my_tcp_cli_sended_to(peer_tcp_srv_addr, pkt_reply_idx(tb));

    // 对端请求资源

    // todo render readfrom

    my_tcp_cli_receive_from(peer_tcp_srv_addr, pkt_req_rs(tb.at(0)));

    // todo 
}