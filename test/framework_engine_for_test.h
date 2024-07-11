#ifndef __NET_FRAMEWORK_ENGINE_FOR_TEST_H_
#define __NET_FRAMEWORK_ENGINE_FOR_TEST_H_

#include <vector>
#include <map>
#include <queue>

#include <cstring>

#include "exc/not_found_exc.h"
#include "framework/net/net_framework.h"
#include "framework/timer/timer_trigger_framework.h"

using namespace std;

class buf_data
{
private:
    uint8_t *data_;
    uint64_t size_;

public:
    buf_data(uint8_t *data, uint64_t size);
    ~buf_data();

    uint8_t *data();
    uint64_t size();
};

class OutputStreamForTest : public OutputStream
{
private:
    queue<shared_ptr<buf_data>> bufs_;

public:
    OutputStreamForTest(/* args */) {}
    ~OutputStreamForTest();

    void write(uint8_t *data, uint64_t size) override;
    std::shared_ptr<buf_data> front();
    void pop(uint16_t size = 1);
    void clear();
};

class NetFrameworkEngineForTest : public NetframeworkEngine
{
private:
    map<NetAddr, shared_ptr<TcpServer>> tcpsrv_;
    map<NetAddr, shared_ptr<UdpServer>> udpsrv_;
    map<NetAddr, shared_ptr<TcpCli>> tcpcli_;
    map<NetAddr, shared_ptr<UdpCli>> udpcli_;

public:
    NetFrameworkEngineForTest(/* args */) {}
    ~NetFrameworkEngineForTest();

    std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) override;
    std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) override;
    std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &addr) override;
    std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &addr) override;

    void unregisterUdpCli(const NetAddr &addr) override;

    std::shared_ptr<TcpCli> findTcpCli(const NetAddr &addr) override;

    std::shared_ptr<NetAbility> queryTcpSerNetAbility(const NetAddr &addr);
    std::shared_ptr<NetAbility> queryTcpCliNetAbility(const NetAddr &addr);
    std::shared_ptr<NetAbility> queryUdpSerNetAbility(const NetAddr &addr);
    std::shared_ptr<NetAbility> queryUdpCliNetAbility(const NetAddr &addr);
    std::shared_ptr<NetAbility> queryNetAbility(const NetAddr &addr);
};

class TimerFrameworkEngineForTest : public TimerFrameworkEngine
{
public:
    TimerFrameworkEngineForTest(/* args */) {}
    ~TimerFrameworkEngineForTest() {}
    void run() {}

    std::shared_ptr<Trigger> findTrigger(uint16_t id);

    void tick(uint64_t since_last);
};

#endif