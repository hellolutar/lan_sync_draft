#ifndef __NET_FRAMEWORK_IMPL_WITH_EVENT_H_
#define __NET_FRAMEWORK_IMPL_WITH_EVENT_H_

#include <map>
#include <vector>

#include <event2/event.h>

#include "framework/impl/net/event_wrap.h"
#include "framework/itf/net/net_framework_engine.h"

class NetFrameworkEngineBaseEvent : public NetframeworkEngine, public std::enable_shared_from_this<NetFrameworkEngineBaseEvent>
{
private:
    std::shared_ptr<EventBaseWrap> base_;
    std::vector<std::shared_ptr<event>> events_; // only persist event need to add
    std::map<std::string, std::shared_ptr<Connection>> ctxs_;
    bool init_ = false;

    void init_check();

public:
    NetFrameworkEngineBaseEvent(std::shared_ptr<EventBaseWrap> b)
        : base_(b){};
    ~NetFrameworkEngineBaseEvent()
    {
        base_ = nullptr;
        events_.clear();
    };

    std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) override;
    std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) override;
    std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &peer) override;
    std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &peer) override;
    void addConn(std::shared_ptr<Connection>) override;
    void start() override;
    void shutdown() override;
    void unRegister(const NetAddr &addr) override;

    void unregisterUdpCli(const NetAddr &addr) override;
    std::shared_ptr<TcpCli> findTcpCli(const NetAddr &addr) override;
};

#endif