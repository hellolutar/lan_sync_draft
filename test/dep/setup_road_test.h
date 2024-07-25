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

class PropertiesParseForTest : public PropertiesParse
{
public:
    PropertiesParseForTest(/* args */) {}
    PropertiesParseForTest(string filepath) : PropertiesParse(filepath) {};
    ~PropertiesParseForTest() {}

    void setProperties(const map<string, string> &p)
    {
        properties = p;
    }
};

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

    const std::vector<Task> getAllTasks() const;

    const std::vector<Task> getTasksByStatus(TaskStatusEnum st) const;

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
    shared_ptr<NetworkAdapterForTest> na_;

    NetAddr my_udp_cli_addr_{"0.0.0.10:18080", TransportType::UDP};
    NetAddr my_udp_srv_addr_{"0.0.0.10:" + to_string(default_udp_srv_port), TransportType::UDP};
    NetAddr my_tcp_cli_addr_{"0.0.0.10:18080", TransportType::TCP};
    NetAddr my_tcp_srv_addr_{"0.0.0.10:" + to_string(default_tcp_srv_port), TransportType::TCP};

    NetAddr peer_udp_cli_addr{peer_udp_cli_addr_str, TransportType::UDP};
    NetAddr peer_udp_srv_addr{peer_udp_srv_addr_str, TransportType::UDP};
    NetAddr peer_tcp_cli_addr{peer_tcp_cli_addr_str, TransportType::TCP};
    NetAddr peer_tcp_srv_addr{peer_tcp_srv_addr_str, TransportType::TCP};

    void SetUp() override;

    void TearDown() override {};

    void recv(std::shared_ptr<NetAbility> net, const NetAddr &peer, LanSyncPkt &pkt);

    optional<LanSyncPkt> popPktFromOs(std::shared_ptr<NetAbility> net);

    optional<LanSyncPkt> justReadPktFromOs(std::shared_ptr<NetAbility> net);

public:
    void my_udp_srv_receive_from(NetAddr &peer, LanSyncPkt &&pkt);

    void my_tcp_srv_receive_from(NetAddr &peer, LanSyncPkt &&pkt);

    void my_tcp_srv_receive_reply_rs_from(NetAddr &peer);

    void my_tcp_cli_receive_from(NetAddr &peer, LanSyncPkt &&pkt);

    void assert_my_tcp_cli_sended_to(const NetAddr &peer, LanSyncPkt &&pkt);

    void assert_my_tcp_cli_notsend_any_pkt_to(const NetAddr &peer);

    void assert_my_tcp_cli_sended_replyRs_to(const NetAddr &peer);

    void assert_my_udp_cli_sended_to(const NetAddr &peer, LanSyncPkt &&pkt);

    void assert_my_tcp_srv_sended_to(const NetAddr &peer, LanSyncPkt &&pkt);

    void assert_my_tcp_srv_sended_getrs_to(const NetAddr &peer);

    void renderReadFrom();
};

#endif