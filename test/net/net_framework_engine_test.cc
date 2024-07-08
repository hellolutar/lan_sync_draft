#include <gtest/gtest.h>

#include "net_framework_engine_for_test.h"

using namespace std;

const string test_case_worlds = "hello world";

class TcpServerDemoLogic : public Logic
{
private:
    string buf;

public:
    TcpServerDemoLogic(/* args */) {}
    ~TcpServerDemoLogic() {}

    const uint64_t isExtraAllDataNow(uint8_t *data, uint64_t data_len) const override
    {
        return data_len == test_case_worlds.size();
    };
    void recv(const NetAddr &peer, uint8_t *data, uint64_t size) override
    {
        string world(reinterpret_cast<char *>(data));
        buf = world;
    }

    const string &getBuf() const
    {
        return buf;
    }
};

TEST(NetFrameworkEngine, tcpCli)
{
    auto ep_ = make_shared<NetFrameworkEngineForTest>();
    NetframeworkEngine::init(ep_);

    auto engine = NetframeworkEngine::getEngine();
    auto addr = NetAddr("192.168.233.1:8080");
    auto srv = engine->addTcpServer(addr);
    auto logic = std::make_shared<TcpServerDemoLogic>();
    srv->bind(logic);

    auto na = ep_->queryNetAbility(addr);

    na->recv(addr, reinterpret_cast<uint8_t *>(const_cast<char *>(test_case_worlds.data())), test_case_worlds.size());

    ASSERT_EQ(test_case_worlds, logic->getBuf());
}