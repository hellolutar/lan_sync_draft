#include <gtest/gtest.h>

#include "net_framework_engine_for_test.h"
#include "end_point.h"
#include "proto/lan_share_protocol.h"
#include "buf/buf_base_on_event.h"

using namespace std;

class EndpointForTest : public Endpoint
{
private:
public:
    EndpointForTest(/* args */) {}
    ~EndpointForTest() {}

    std::shared_ptr<NetworkAdapter> getNetworkAdapter()
    {
        return na_;
    }
};

class EndPointTestCaseSimple : public testing::Test
{
protected:
    EndpointForTest ed_;
    shared_ptr<NetFrameworkEngineForTest> ep_;
    NetAddr local_addr_;

    void SetUp() override
    {
        printf("SetUp \n");
        ep_ = make_shared<NetFrameworkEngineForTest>();
        Netframework::init(ep_);
        ed_.run();
    }

    void TearDown() override
    {
        printf("TearDown \n");
    }

public:
    void env(NetAddr local)
    {
        local_addr_ = local;
    }

    void udp_cli_conn(NetAddr &addr)
    {
        auto na = ed_.getNetworkAdapter();
        na->setUpSessionWithPeer(addr);
    }

    void udp_srv_receive(NetAddr &peer, LanSyncPkt &pkt)
    {
        auto ed_udp_srv = ep_->queryUdpSerNetAbility(local_addr_);

        BufBaseonEvent buf;
        pkt.writeTo(buf);
        ed_udp_srv->recv(peer, reinterpret_cast<uint8_t *>(buf.data()), buf.size());
    }

    void assert_tcp_cli_sended(NetAddr &peer, LanSyncPkt &pkt)
    {
        auto ed_tcp_cli = ep_->queryTcpCliNetAbility(peer);
        auto os = dynamic_pointer_cast<OutputStreamForTest>(ed_tcp_cli->getOutputStream());
        auto recvBufs = os->getBufs();
        LanSyncPkt tcpRecvPkt(recvBufs[0]->data());

        ASSERT_EQ(pkt, tcpRecvPkt);
    }

    void assert_udp_cli_sended(NetAddr &peer, LanSyncPkt &pkt)
    {
        auto ed_tcp_cli = ep_->queryUdpSerNetAbility(peer);
        auto os = dynamic_pointer_cast<OutputStreamForTest>(ed_tcp_cli->getOutputStream());
        auto recvBufs = os->getBufs();
        LanSyncPkt tcpRecvPkt(recvBufs[0]->data());

        ASSERT_EQ(pkt, tcpRecvPkt);
    }
};

TEST_F(EndPointTestCaseSimple, server_recv_hello_then_reply_helloack)
{
    auto peerAddr = NetAddr("0.0.0.1:18080");
    env({"192.168.233.1:8080"});

    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO);
    uint16_t port = 18080;
    pkt.setPayload(&port, sizeof(uint16_t));
    udp_srv_receive(peerAddr, pkt);

    pkt = LanSyncPkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK);
    assert_tcp_cli_sended(peerAddr, pkt);
}

// TEST_F(EndPointTestCaseSimple, cli_send_hello_then_recv_helloack)
// {
//     auto peerAddr = NetAddr("0.0.0.2:2000");
//     env({"192.168.233.1:8080"});

//     LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO);
//     uint16_t port = 18080;
//     pkt.setPayload(&port, sizeof(uint16_t));
//     udp_cli_conn(peerAddr);

//     pkt = LanSyncPkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK);
//     assert_udp_cli_sended(peerAddr, pkt);
// }
