#ifndef __NET_FRAMEWORK_IMPL_WITH_EVENT_H_
#define __NET_FRAMEWORK_IMPL_WITH_EVENT_H_

#include <map>
#include <set>
#include <vector>
#include <cerrno>
#include <cstring>
#include <functional>

#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <netinet/tcp.h>

#include "framework/itf/net/net_framework_engine.h"
// #include "utils/logger.h"

class NetFrameworkEngineBaseEvent : public NetframeworkEngine, public std::enable_shared_from_this<NetFrameworkEngineBaseEvent>
{
private:
    std::shared_ptr<event_base> base_;
    std::vector<std::shared_ptr<event>> events_; // only persist event need to add
    std::map<NetAddr, NetAbility> ctxs_;

    void init_check();

public:
    NetFrameworkEngineBaseEvent(){};
    ~NetFrameworkEngineBaseEvent()
    {
        base_ = nullptr;
        events_.clear();
    };

    std::shared_ptr<TcpServer> addTcpServer(const NetAddr &addr) override;
    std::shared_ptr<UdpServer> addUdpServer(const NetAddr &addr) override;
    std::shared_ptr<TcpCli> connectWithTcp(const NetAddr &peer) override;
    std::shared_ptr<UdpCli> connectWithUdp(const NetAddr &peer) override;

    void unregisterUdpCli(const NetAddr &addr) override;
    std::shared_ptr<TcpCli> findTcpCli(const NetAddr &addr) override;

    void run();
    void shutdown();
};

#endif