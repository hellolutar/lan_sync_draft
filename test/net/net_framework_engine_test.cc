#include <gtest/gtest.h>

#include "dep/framework_engine_for_test.h"

using namespace std;

const string test_case_worlds = "hello world";

class TcpServerDemoLogic : public Logic
{
private:
    string buf;

public:
    TcpServerDemoLogic(/* args */) {}
    ~TcpServerDemoLogic() {}

    const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t data_len) const override
    {
        return data_len == test_case_worlds.size();
    };
    void recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size) override
    {
        string world(reinterpret_cast<char *>(data.get()));
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
    Netframework::init(ep_);

    auto engine = Netframework::getEngine();
    auto addr = NetAddr("192.168.233.1:8080");
    auto srv = engine->addTcpServer(addr);
    auto logic = std::make_shared<TcpServerDemoLogic>();
    srv->bind(logic);

    auto na = ep_->queryNetAbility(addr);

    std::shared_ptr<uint8_t[]> sp(new uint8_t[test_case_worlds.size()]());
    memcpy(sp.get(), test_case_worlds.data(), test_case_worlds.size());

    na->recv(addr, sp, test_case_worlds.size());

    ASSERT_EQ(test_case_worlds, logic->getBuf());
}