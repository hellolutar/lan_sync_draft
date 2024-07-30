#include "uint-test/dep/setup_road_test.h"
#include "setup_road_test.h"

const std::vector<Task> TaskManagerForTest::getTasksByStatus(TaskStatusEnum st) const
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

const std::vector<Task> TaskManagerForTest::getAllTasks() const
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

void EndPointTestCaseSimple::SetUp()
{
    printf("SetUp \n");

    rm_ = make_shared<ResourceManagerForTest>();
    tm_ = make_shared<TaskManagerForTest>();

    neg_ = make_shared<NetFrameworkEngineForTest>();
    Netframework::init(neg_);

    teg_ = make_shared<TimerFrameworkEngineForTest>();
    TimerFramework::init(teg_);

    na_ = make_shared<NetworkAdapterForTest>();
    ed_.setNetworkAdapter(na_);

    map<string, string> properties;
    properties[PropertiesParse::DISCOVER_IPS] = "0.0.0.1";
    properties[PropertiesParse::PROTO_DISCOVER_SERVER_UDP_PORT] = "58080";
    shared_ptr<PropertiesParseForTest> p = std::make_shared<PropertiesParseForTest>("");
    p->setProperties(properties);
    shared_ptr<PropertiesParse> a = p;
    ed_.setPropertiesParse(p);

    ed_.setTaskManager(tm_);
    ed_.setResourceManager(rm_);
    ed_.init();

    ed_.run();
}

void EndPointTestCaseSimple::recv(std::shared_ptr<NetAbility> net,
                                  std::shared_ptr<OutputStream> os,
                                  const NetAddr &peer, LanSyncPkt &pkt)
{
    BufBaseonEvent buf;
    pkt.writeTo(buf);

    NetAbilityContext na_ctx(os, peer);
    net->recv(na_ctx, buf.data(), buf.size());
}

optional<LanSyncPkt> EndPointTestCaseSimple::popPktFromOs(std::shared_ptr<OutputStreamForTest> os)
{
    auto recvBuf = os->front();
    if (recvBuf == nullptr)
        return nullopt;
    os->pop();
    return make_optional<LanSyncPkt>(recvBuf->data());
}

optional<LanSyncPkt> EndPointTestCaseSimple::justReadPktFromOs(std::shared_ptr<OutputStreamForTest> os)
{
    if (os->size() == 0)
        return nullopt;

    auto recvBuf = os->front();
    if (recvBuf == nullptr)
        return nullopt;
    return make_optional<LanSyncPkt>(recvBuf->data());
}

void EndPointTestCaseSimple::my_udp_srv_receive_from(NetAddr &peer, LanSyncPkt &&pkt)
{
    auto srv = neg_->queryUdpSerNetAbility(my_udp_srv_addr_);
    recv(srv, srv->os(), peer, pkt);
}
void EndPointTestCaseSimple::my_tcp_srv_receive_from(NetAddr &peer, LanSyncPkt &&pkt)
{
    auto srv = neg_->queryTcpSerNetAbility(my_tcp_srv_addr_);
    auto cli = neg_->queryTcpCliNetAbility(peer);
    recv(srv, cli->os(), peer, pkt);
}

void EndPointTestCaseSimple::my_tcp_srv_receive_reply_rs_from(NetAddr &peer)
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
        recv(srv, srv->os(), peer, pkt);
    }
}

void EndPointTestCaseSimple::my_tcp_cli_receive_from(NetAddr &peer, LanSyncPkt &&pkt)
{
    peer.setType(TransportType::TCP);
    auto srv = neg_->queryTcpCliNetAbility(peer);
    recv(srv, srv->os(), peer, pkt);
}

void EndPointTestCaseSimple::assert_my_tcp_cli_sended_to(const NetAddr &peer, LanSyncPkt &&pkt)
{
    auto cli = neg_->queryTcpCliNetAbility(peer);
    auto sentPkt = popPktFromOs(reinterpret_pointer_cast<OutputStreamForTest>(cli->os()));
    ASSERT_TRUE(sentPkt.has_value());
    auto &p = sentPkt.value();
    ASSERT_EQ(pkt, p);
}

void EndPointTestCaseSimple::assert_my_tcp_cli_notsend_any_pkt_to(const NetAddr &peer)
{
    auto cli = neg_->queryTcpCliNetAbility(peer);
    auto sentPkt = justReadPktFromOs(reinterpret_pointer_cast<OutputStreamForTest>(cli->os()));
    ASSERT_FALSE(sentPkt.has_value());
}

void EndPointTestCaseSimple::assert_my_tcp_cli_sended_replyRs_to(const NetAddr &peer)
{
    auto cli = neg_->queryTcpCliNetAbility(peer);

    uint16_t limit = 100;
    uint16_t i = 0;
    while (true)
    {
        ASSERT_LT(i++, limit);

        auto sentPktOpt = popPktFromOs(reinterpret_pointer_cast<OutputStreamForTest>(cli->os()));
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

void EndPointTestCaseSimple::assert_my_udp_cli_sended_to(const NetAddr &peer, LanSyncPkt &&pkt)
{
    auto cli = neg_->queryUdpCliNetAbility(peer);
    auto sentPkt = popPktFromOs(reinterpret_pointer_cast<OutputStreamForTest>(cli->os()));
    ASSERT_TRUE(sentPkt.has_value());
    ASSERT_EQ(pkt, sentPkt.value());
}

void EndPointTestCaseSimple::assert_my_tcp_srv_sended_to(const NetAddr &peer, LanSyncPkt &&pkt)

{
    // 根据peer查找session，然后session.write，故tcp_srv的send的内容，存放在peer的session的outputstream中
    auto cli = neg_->queryTcpCliNetAbility(peer);
    auto os = reinterpret_pointer_cast<OutputStreamForTest>(cli->os());
    auto sentPktOpt = popPktFromOs(os);
    ASSERT_TRUE(sentPktOpt.has_value());
    auto sendPkt = sentPktOpt.value();
    ASSERT_EQ(pkt, sendPkt);
}

void EndPointTestCaseSimple::assert_my_tcp_srv_sended_getrs_to(const NetAddr &peer)

{
    // 根据peer查找session，然后session.write，故tcp_srv的send的内容，存放在peer的session的outputstream中
    auto cli = neg_->queryTcpCliNetAbility(peer);
    auto sentPkt = justReadPktFromOs(reinterpret_pointer_cast<OutputStreamForTest>(cli->os()));
    ASSERT_TRUE(sentPkt.has_value());
    ASSERT_EQ(sentPkt.value().getType(), lan_sync_type_enum::LAN_SYNC_TYPE_GET_RESOURCE);
}

void EndPointTestCaseSimple::renderReadFrom()
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


void EndPointTestCaseSimple::set_validres(bool b){
    rm_->setValidRes(b);
}