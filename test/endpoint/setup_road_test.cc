#include <gtest/gtest.h>

#include "framework_engine_for_test.h"
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
    shared_ptr<NetFrameworkEngineForTest> neg_;
    shared_ptr<TimerFrameworkEngineForTest> teg_;
    NetAddr local_addr_;

    void SetUp() override
    {
        printf("SetUp \n");
        neg_ = make_shared<NetFrameworkEngineForTest>();
        Netframework::init(neg_);

        teg_ = make_shared<TimerFrameworkEngineForTest>();
        TimerFramework::init(teg_);

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

    void udp_srv_receive(NetAddr &peer, LanSyncPkt &pkt)
    {
        auto ed_udp_srv = neg_->queryUdpSerNetAbility(local_addr_);

        BufBaseonEvent buf;
        pkt.writeTo(buf);
        ed_udp_srv->recv(peer, reinterpret_cast<uint8_t *>(buf.data()), buf.size());
    }

    void assert_tcp_cli_sended(NetAddr &peer, LanSyncPkt &pkt)
    {
        peer.setType(TransportType::TCP);
        auto cli = neg_->queryTcpCliNetAbility(peer);
        auto os = dynamic_pointer_cast<OutputStreamForTest>(cli->getOutputStream());
        auto recvBuf = os->front();
        LanSyncPkt recvPkt(recvBuf->data());

        ASSERT_EQ(pkt, recvPkt);
    }

    void assert_udp_cli_sended(NetAddr &peer, LanSyncPkt &pkt)
    {
        peer.setType(TransportType::UDP);
        auto cli = neg_->queryUdpCliNetAbility(peer);
        auto os = dynamic_pointer_cast<OutputStreamForTest>(cli->getOutputStream());
        auto recvBuf = os->front();
        LanSyncPkt recvPkt(recvBuf->data());

        ASSERT_EQ(pkt, recvPkt);
    }
};

TEST_F(EndPointTestCaseSimple, server_recv_hello_then_reply_helloack)
{
    env({"192.168.233.1:8080"});
    auto peerAddr = NetAddr("0.0.0.1:18080");

    LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO);
    uint16_t port = 18080;
    pkt.setPayload(&port, sizeof(uint16_t));
    udp_srv_receive(peerAddr, pkt);

    pkt = LanSyncPkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO_ACK);
    assert_tcp_cli_sended(peerAddr, pkt);
}

TEST_F(EndPointTestCaseSimple, trigger_send_hello_with_udp)
{
    env({"192.168.233.1:8080"});
    auto peerAddr = NetAddr("0.0.0.1:18080");

    teg_->tick(2000);

    LanSyncPkt pkt{lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_HELLO};
    uint16_t port = 8080;
    pkt.setPayload(&port, sizeof(uint16_t));
    assert_udp_cli_sended(peerAddr, pkt);
}
