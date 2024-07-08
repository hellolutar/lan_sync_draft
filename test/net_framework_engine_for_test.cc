#include "net_framework_engine_for_test.h"

void OutputStreamForTest::write(uint8_t *data, uint64_t size)
{
    auto b = make_shared<buf_data>(data, size);
    bufs_.push_back(b);
}

vector<shared_ptr<buf_data>> &OutputStreamForTest::getBufs()
{
    return bufs_;
}

std::shared_ptr<TcpServer> NetFrameworkEngineForTest::addTcpServer(const NetAddr &addr)
{
    auto srv = std::make_shared<TcpServer>(addr);
    auto os = std::make_shared<OutputStreamForTest>();
    srv->setOutputStream(os);
    tcpsrv_[addr] = srv;
    return srv;
}

std::shared_ptr<UdpServer> NetFrameworkEngineForTest::addUdpServer(const NetAddr &addr)
{
    auto srv = std::make_shared<UdpServer>(addr);
    auto os = std::make_shared<OutputStreamForTest>();
    srv->setOutputStream(os);
    udpsrv_[addr] = srv;
    return srv;
}

std::shared_ptr<TcpCli> NetFrameworkEngineForTest::connectWithTcp(const NetAddr &addr)
{
    auto cli = std::make_shared<TcpCli>(addr);
    auto os = std::make_shared<OutputStreamForTest>();
    cli->setOutputStream(os);
    tcpcli_[addr] = cli;
    return cli;
}

std::shared_ptr<UdpCli> NetFrameworkEngineForTest::connectWithUdp(const NetAddr &addr)
{
    auto cli = std::make_shared<UdpCli>(addr);
    auto os = std::make_shared<OutputStreamForTest>();
    cli->setOutputStream(os);
    udpcli_[addr] = cli;
    return cli;
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryTcpSerNetAbility(const NetAddr &addr)
{
    if (tcpsrv_.find(addr) != tcpsrv_.end())
        return tcpsrv_.find(addr)->second;

    throw "not found";
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryTcpCliNetAbility(const NetAddr &addr)
{
    if (tcpcli_.find(addr) != tcpcli_.end())
        return tcpcli_.find(addr)->second;

    throw "not found";
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryUdpSerNetAbility(const NetAddr &addr)
{
    if (udpsrv_.find(addr) != udpsrv_.end())
        return udpsrv_.find(addr)->second;

    throw "not found";
}

std::shared_ptr<NetAbility> NetFrameworkEngineForTest::queryNetAbility(const NetAddr &addr)
{
    if (udpsrv_.find(addr) != udpsrv_.end())
        return udpsrv_.find(addr)->second;

    if (tcpsrv_.find(addr) != tcpsrv_.end())
        return tcpsrv_.find(addr)->second;

    if (tcpcli_.find(addr) != tcpcli_.end())
        return tcpcli_.find(addr)->second;

    if (udpcli_.find(addr) != udpcli_.end())
        return udpcli_.find(addr)->second;

    throw "not found";
}
