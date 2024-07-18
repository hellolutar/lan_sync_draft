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

TEST(NetAddrTest, str2Test)
{
    ASSERT_EQ("udp://0.0.225.202:0", NetAddr("0.0.225.202").str());
}

void ipv4_generateor(string &pre, int pos)
{
    if (pos > 3)
    {
        // cout << NetAddr(pre).str() << endl;
        ASSERT_EQ("udp://" + pre + ":0", NetAddr(pre).str()) << "pos:" << pos << "\texpected:" << pre << "\tactual:" << NetAddr(pre).str();
        return;
    }

    for (size_t i = 0; i <= 255; i++)
    {
        auto ip = pre;
        auto ip_pos = pos;
        if (pre.size() > 0)
            ip = ip + "." + to_string(i);
        else
            ip = to_string(i);
        ipv4_generateor(ip, ++ip_pos);

        if (pos == 0)
            cout << "test: " << ip << ".*.*.*" << endl;
    }
}

TEST(NetAddrTest, str_test_simple)
{
    cout << "test NetAddr.str() -> start!" << endl;
    for (size_t i = 0; i <= 255; i++)
    {
        string each = to_string(i);
        string ip = each + "." + each + "." + each + "." + each;
        ASSERT_EQ("udp://" + ip + ":0", NetAddr(ip).str()) << "\texpected:" << ip << "\tactual:" << NetAddr(ip).str();
    }

    cout << "test NetAddr.str() -> done!" << endl;
}

// TEST(NetAddrTest, str_test_complete)
// {
//     cout << "test NetAddr.str() -> start!" << endl;
//     string ip = "";
//     ipv4_generateor(ip, 0);
//     cout << "test NetAddr.str() -> done!" << endl;
// }
