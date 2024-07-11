#ifndef __SETUP_ROAD_TEST_H_
#define __SETUP_ROAD_TEST_H_

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
    NetAddr my_udp_cli_addr_{"0.0.0.10:18080", TransportType::UDP};
    NetAddr my_udp_srv_addr_{"0.0.0.10:8080", TransportType::UDP};
    NetAddr my_tcp_cli_addr_{"0.0.0.10:18080", TransportType::TCP};
    NetAddr my_tcp_srv_addr_{"0.0.0.10:8080", TransportType::TCP};

    void SetUp() override
    {
        printf("SetUp \n");
        env({":18080"});

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

    void recv(std::shared_ptr<NetAbility> net, NetAddr &peer, LanSyncPkt &pkt)
    {
        BufBaseonEvent buf;
        pkt.writeTo(buf);
        net->recv(peer, reinterpret_cast<uint8_t *>(buf.data()), buf.size());
    }

    LanSyncPkt popPktFromOs(std::shared_ptr<NetAbility> net)
    {
        auto os = dynamic_pointer_cast<OutputStreamForTest>(net->getOutputStream());
        auto recvBuf = os->front();
        os->pop();
        LanSyncPkt recvPkt(recvBuf->data());
        return recvPkt;
    }

public:
    void env(NetAddr local)
    {
    }

    void my_udp_srv_receive(NetAddr &peer, LanSyncPkt pkt)
    {
        peer.setType(TransportType::UDP);
        auto srv = neg_->queryUdpSerNetAbility(my_udp_srv_addr_);
        recv(srv, peer, pkt);
    }
    void my_tcp_srv_receive(NetAddr &peer, LanSyncPkt pkt)
    {
        peer.setType(TransportType::TCP);
        auto srv = neg_->queryTcpSerNetAbility(my_tcp_srv_addr_);
        recv(srv, peer, pkt);
    }
    void my_tcp_cli_receive(NetAddr &peer, LanSyncPkt pkt)
    {
        peer.setType(TransportType::TCP);
        auto srv = neg_->queryTcpCliNetAbility(my_tcp_cli_addr_);
        recv(srv, peer, pkt);
    }

    void assert_my_tcp_cli_sended(NetAddr &peer, LanSyncPkt pkt)
    {
        peer.setType(TransportType::TCP);
        auto cli = neg_->queryTcpCliNetAbility(peer);
        ASSERT_EQ(pkt, popPktFromOs(cli));
    }

    void assert_my_udp_cli_sended(NetAddr &peer, LanSyncPkt pkt)
    {
        peer.setType(TransportType::UDP);
        auto cli = neg_->queryUdpCliNetAbility(peer);
        ASSERT_EQ(pkt, popPktFromOs(cli));
    }
};

#endif