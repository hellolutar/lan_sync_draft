#ifndef __NET_FRAMEWORK_ENGINE_FOR_TEST_H_
#define __NET_FRAMEWORK_ENGINE_FOR_TEST_H_

#include <vector>
#include <map>

#include <cstring>

#include "net/net_framework_engine.h"

using namespace std;

class buf_data
{
private:
    uint8_t *data_;
    uint64_t size_;

public:
    buf_data(uint8_t *data, uint64_t size)
    {
        data_ = new uint8_t[size];
        size_ = size;
        memcpy(data_, data, size);
    }
    ~buf_data()
    {
        delete[] data_;
    }

    uint8_t *data()
    {
        return data_;
    }
    uint64_t size()
    {
        return size_;
    }
};

class OutputStreamForTest : public OutputStream
{
private:
    vector<shared_ptr<buf_data>> bufs_;

public:
    OutputStreamForTest(/* args */) {}
    ~OutputStreamForTest() {}

    void write(uint8_t *data, uint64_t size) override;
    vector<shared_ptr<buf_data>> &getBufs();
};

class NetFrameworkEngineForTest : public NetFramework
{
private:
    map<NetAddr, shared_ptr<TcpServer>> tcpsrv_;
    map<NetAddr, shared_ptr<UdpServer>> udpsrv_;
    map<NetAddr, shared_ptr<TcpCli>> tcpcli_;
    map<NetAddr, shared_ptr<UdpCli>> udpcli_;

public:
    NetFrameworkEngineForTest(/* args */) {}
    ~NetFrameworkEngineForTest() {
        tcpsrv_.clear();
        udpsrv_.clear();
        tcpcli_.clear();
        udpcli_.clear();
    }

    std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) override;
    std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) override;
    std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &addr) override;
    std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &addr) override;

    std::shared_ptr<NetAbility> queryTcpSerNetAbility(const NetAddr &addr);
    std::shared_ptr<NetAbility> queryTcpCliNetAbility(const NetAddr &addr);
    std::shared_ptr<NetAbility> queryUdpSerNetAbility(const NetAddr &addr);
    std::shared_ptr<NetAbility> queryNetAbility(const NetAddr &addr);
};

#endif