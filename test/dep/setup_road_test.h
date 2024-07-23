#ifndef __SETUP_ROAD_TEST_H_
#define __SETUP_ROAD_TEST_H_

#include <gtest/gtest.h>

#include <cstring>

#include "end_point.h"
#include "dep/framework_engine_for_test.h"
#include "proto/lan_share_protocol.h"
#include "buf/buf_base_on_event.h"
#include "dep/test_sync_proto.h"

using namespace std;

class TaskManagerForTest : public TaskManager
{
private:
    /* data */
public:
    TaskManagerForTest(/* args */) {}
    ~TaskManagerForTest() {}

    const std::map<std::string, std::vector<Task>> &getTasks() const
    {
        return tasks_;
    }

    const std::vector<Task> getAllTasks() const
    {
        std::vector<Task> ret;
        for (auto &&kv : tasks_)
        {
            for (auto &&t : kv.second)
            {
                ret.push_back(t);
            }
        }
        return ret;
    }

    const std::vector<Task> getTasksByStatus(TaskStatusEnum st) const
    {
        std::vector<Task> ret;
        for (auto &&kv : tasks_)
        {
            for (auto &&t : kv.second)
            {
                if (t.getStatus() == st)
                    ret.push_back(t);
            }
        }
        return ret;
    }

    const std::vector<Task> getSyncingTasks() const
    {
        return getTasksByStatus(TaskStatusEnum::Syning);
    }

    const std::vector<Task> getSuccessTasks() const
    {
        return getTasksByStatus(TaskStatusEnum::Success);
    }
};

class EndpointForTest : public Endpoint
{
private:
public:
    EndpointForTest() {}

    ~EndpointForTest() {}

    std::shared_ptr<NetworkAdapter> getNetworkAdapter()
    {
        return na_;
    }

    std::shared_ptr<TaskCoordinatorTrigger> getTaskCoordinatorTrigger()
    {
        return coor_trg_;
    }

    std::shared_ptr<LogicCore> getLogicCore() { return core_; }
};

class EndPointTestCaseSimple : public testing::Test
{
protected:
    EndpointForTest ed_;
    shared_ptr<NetFrameworkEngineForTest> neg_;
    shared_ptr<TimerFrameworkEngineForTest> teg_;
    shared_ptr<ResourceManagerForTest> rm_;
    shared_ptr<TaskManagerForTest> tm_;

    NetAddr my_udp_cli_addr_{"0.0.0.10:18080", TransportType::UDP};
    NetAddr my_udp_srv_addr_{"0.0.0.10:" +  to_string(default_udp_srv_port), TransportType::UDP};
    NetAddr my_tcp_cli_addr_{"0.0.0.10:18080", TransportType::TCP};
    NetAddr my_tcp_srv_addr_{"0.0.0.10:" +  to_string(default_tcp_srv_port), TransportType::TCP};

    NetAddr peer_udp_cli_addr{peer_udp_cli_addr_str, TransportType::UDP};
    NetAddr peer_udp_srv_addr{peer_udp_srv_addr_str, TransportType::UDP};
    NetAddr peer_tcp_cli_addr{peer_tcp_cli_addr_str, TransportType::TCP};
    NetAddr peer_tcp_srv_addr{peer_tcp_srv_addr_str, TransportType::TCP};

    void SetUp() override
    {
        printf("SetUp \n");

        rm_ = make_shared<ResourceManagerForTest>();
        tm_ = make_shared<TaskManagerForTest>();

        neg_ = make_shared<NetFrameworkEngineForTest>();
        Netframework::init(neg_);

        teg_ = make_shared<TimerFrameworkEngineForTest>();
        TimerFramework::init(teg_);

        auto na = make_shared<NetworkAdapterForTest>();
        ed_.setNetworkAdapter(na);

        ed_.setTaskManager(tm_);
        ed_.init();
        ed_.getLogicCore()->setResourceManager(rm_);

        ed_.run();
    }

    void TearDown() override
    {
        printf("TearDown \n");
    }

    void recv(std::shared_ptr<NetAbility> net, const NetAddr &peer, LanSyncPkt &pkt)
    {
        BufBaseonEvent buf;
        pkt.writeTo(buf);
        net->recv(peer, buf.data(), buf.size());
    }

    optional<LanSyncPkt> popPktFromOs(std::shared_ptr<NetAbility> net)
    {
        auto &os = reinterpret_cast<const std::unique_ptr<OutputStreamForTest> &>(net->getOutputStream());

        auto recvBuf = os->front();
        if (recvBuf == nullptr)
            return nullopt;
        os->pop();
        return make_optional<LanSyncPkt>(recvBuf->data());
    }
    LanSyncPkt justReadPktFromOs(std::shared_ptr<NetAbility> net)
    {
        auto &os = reinterpret_cast<const std::unique_ptr<OutputStreamForTest> &>(net->getOutputStream());
        auto recvBuf = os->front();
        LanSyncPkt recvPkt(recvBuf->data());
        return recvPkt;
    }

public:
    void env(NetAddr local)
    {
    }

    void my_udp_srv_receive_from(NetAddr &peer, LanSyncPkt &&pkt)
    {
        auto srv = neg_->queryUdpSerNetAbility(my_udp_srv_addr_);
        recv(srv, peer, pkt);
    }
    void my_tcp_srv_receive_from(NetAddr &peer, LanSyncPkt &&pkt)
    {
        auto srv = neg_->queryTcpSerNetAbility(my_tcp_srv_addr_);
        recv(srv, peer, pkt);
    }
    void my_tcp_srv_receive_reply_rs_from(NetAddr &peer)
    {
        auto srv = neg_->queryTcpSerNetAbility(my_tcp_srv_addr_);
        auto syncingTasks = tm_->getSyncingTasks();
        ASSERT_LT(syncingTasks.size(), DOWNLOAD_LIMIT + 1);

        for (auto &&t : syncingTasks)
        {
            auto &blk = t.getBlock();
            LanSyncPkt pkt(lan_sync_version::VER_0_1, lan_sync_type_enum::LAN_SYNC_TYPE_REPLY_RESOURCE);
            if (blk.size() > 0)
            {
                string range_hdr = Range(blk.start, blk.end).to_string();
                pkt.addXheader(XHEADER_URI, t.getUri());
                pkt.addXheader(XHEADER_RANGE, range_hdr);

                auto str = gen_u8_array(blk.size());
                pkt.setPayload(str.get(), blk.size());
            }
            recv(srv, peer, pkt);
        }
    }
    void my_tcp_cli_receive_from(NetAddr &peer, LanSyncPkt &&pkt)
    {
        peer.setType(TransportType::TCP);
        auto srv = neg_->queryTcpCliNetAbility(peer);
        recv(srv, peer, pkt);
    }

    void assert_my_tcp_cli_sended_to(const NetAddr &peer, LanSyncPkt &&pkt)
    {
        auto cli = neg_->queryTcpCliNetAbility(peer);
        auto sentPkt = popPktFromOs(cli);
        ASSERT_TRUE(sentPkt.has_value());
        auto &p = sentPkt.value();
        ASSERT_EQ(pkt, p);
    }

    void assert_my_tcp_cli_sended_replyRs_to(const NetAddr &peer)
    {
        auto cli = neg_->queryTcpCliNetAbility(peer);

        uint16_t limit = 100;
        uint16_t i = 0;
        while (true)
        {
            ASSERT_LT(i++, limit);

            auto sentPktOpt = popPktFromOs(cli);
            if (!sentPktOpt.has_value())
                return;

            auto sentPkt = sentPktOpt.value();

            auto uri_opt = sentPkt.queryXheader(XHEADER_URI);
            auto range_opt = sentPkt.queryXheader(XHEADER_RANGE);
            Range range(range_opt.value());
            uint64_t offset = range.getStart();

            uint64_t size = min(BLOCK_SIZE, (range.getEnd() - offset));

            Block b(offset, offset + size);
            Range r(b.start, b.end);

            auto data_opt = rm_->queryReadFromHistory({uri_opt.value(), b});

            ASSERT_TRUE(data_opt.has_value());

            LanSyncPkt p = {lan_sync_version::VER_0_1, LAN_SYNC_TYPE_REPLY_RESOURCE};
            p.addXheader(XHEADER_RANGE, r.to_string());
            p.addXheader(XHEADER_URI, uri_opt.value());
            p.setPayload(data_opt.value().get(), b.size());

            ASSERT_EQ(sentPkt, p);

            offset += b.size();
        }
    }

    void assert_my_udp_cli_sended_to(const NetAddr &peer, LanSyncPkt &&pkt)
    {
        auto cli = neg_->queryUdpCliNetAbility(peer);
        auto sentPkt = popPktFromOs(cli);
        ASSERT_TRUE(sentPkt.has_value());
        ASSERT_EQ(pkt, sentPkt.value());
    }

    void assert_my_tcp_srv_sended_to(const NetAddr &peer, LanSyncPkt &&pkt)
    {
        // 根据peer查找session，然后session.write，故tcp_srv的send的内容，存放在peer的session的outputstream中
        auto cli = neg_->queryTcpCliNetAbility(peer);
        auto sentPkt = popPktFromOs(cli);
        ASSERT_TRUE(sentPkt.has_value());
        ASSERT_EQ(pkt, sentPkt.value());
    }
    void assert_my_tcp_srv_sended_getrs_to(const NetAddr &peer)
    {
        // 根据peer查找session，然后session.write，故tcp_srv的send的内容，存放在peer的session的outputstream中
        auto cli = neg_->queryTcpCliNetAbility(peer);
        auto sentPkt = justReadPktFromOs(cli);
        ASSERT_EQ(sentPkt.getType(), lan_sync_type_enum::LAN_SYNC_TYPE_GET_RESOURCE);
    }

    void renderReadFrom()
    {
        auto uri_tasks = tm_->getTasks();
        for (auto &&kv : uri_tasks)
        {
            auto tasks = kv.second;
            for (auto &&t : tasks)
            {
                uri_block ub{t.getUri(), t.getBlock()};
                rm_->setReadFrom(ub, gen_u8_array(t.getBlock().size()));
            }
        }
    }

    LanSyncPkt pkt_hello(void *payload = nullptr, uint64_t size = 0);
    LanSyncPkt pkt_hello_ack(void *payload = nullptr, uint64_t size = 0);
    LanSyncPkt pkt_req_idx();
    LanSyncPkt pkt_reply_idx(vector<Resource> tb);
    LanSyncPkt pkt_req_rs(Resource r);
};

#endif