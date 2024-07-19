#ifndef __RECV_BASE_EVENT_H_
#define __RECV_BASE_EVENT_H_

#include "framework/itf/net/net_framework_engine.h"
#include "framework/impl/net/outputstream_base_event.h"
#include "framework/impl/net/outputstream_for_udp.h"

class BaseEngineDto
{
private:
    std::shared_ptr<EventBaseWrap> base_;
    std::shared_ptr<NetframeworkEngine> engine_;

public:
    BaseEngineDto(std::shared_ptr<EventBaseWrap> base, std::shared_ptr<NetframeworkEngine> engine)
        : base_(base), engine_(engine) {}
    ~BaseEngineDto()
    {
        base_ = nullptr;
        engine_ = nullptr;
    }

    std::shared_ptr<EventBaseWrap> getBase()
    {
        return base_;
    }
    std::shared_ptr<NetframeworkEngine> getEngine()
    {
        return engine_;
    }
};

class TcpConn : public ConnectionBaseEvent
{
private:
    std::shared_ptr<NetAbility> ne_;

public:
    TcpConn(NetAddr peer, std::shared_ptr<NetAbility> ne)
        : ConnectionBaseEvent(nullptr, peer), ne_(ne) {}
    ~TcpConn() {}

    NetAddr getPeer()
    {
        return addr_;
    }
    std::shared_ptr<NetAbility> getNe()
    {
        return ne_;
    }
};

class UdpConn : public ConnectionBaseEvent
{
private:
    std::shared_ptr<NetAbility> ne_;
    int sock_;

public:
    UdpConn(NetAddr peer, std::shared_ptr<NetAbility> ne, int sk)
        : ConnectionBaseEvent(nullptr, peer), ne_(ne), sock_(sk) {}
    ~UdpConn() {}

    std::shared_ptr<NetAbility> getNe()
    {
        return ne_;
    }

    int getSock()
    {
        return sock_;
    }

    NetAddr getPeer()
    {
        return addr_;
    }
};

void event_cb(struct bufferevent *bev, short events, void *ctx);
void write_cb(struct bufferevent *bev, void *data);
void read_cb(struct bufferevent *bev, void *ctx);
void udp_read_cb(evutil_socket_t fd, short events, void *ctx);
void tcp_accept(evutil_socket_t listener, short event, void *ctx);

#endif