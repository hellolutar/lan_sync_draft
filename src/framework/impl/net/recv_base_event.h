#ifndef __RECV_BASE_EVENT_H_
#define __RECV_BASE_EVENT_H_

#include <map>
#include <set>
#include <vector>
#include <cerrno>
#include <cstring>
#include <functional>
#include <memory>
#include <cassert>

#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <netinet/tcp.h>

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

class TcpCbDto
{
private:
    NetAddr peer_;
    std::shared_ptr<NetAbility> ne_;

public:
    TcpCbDto(NetAddr peer, std::shared_ptr<NetAbility> ne)
        : peer_(peer), ne_(ne) {}
    ~TcpCbDto() {}

    NetAddr getPeer()
    {
        return peer_;
    }
    std::shared_ptr<NetAbility> getNe()
    {
        return ne_;
    }
};

class UdpCbDto
{
private:
    std::shared_ptr<NetAbility> ne_;
    int sock_;

public:
    UdpCbDto(std::shared_ptr<NetAbility> ne, int sk)
        : ne_(ne), sock_(sk) {}
    ~UdpCbDto() {}

    std::shared_ptr<NetAbility> getNe()
    {
        return ne_;
    }

    int getSock()
    {
        return sock_;
    }
};

void event_cb(struct bufferevent *bev, short events, void *ctx);
void write_cb(struct bufferevent *bev, void *data);
void read_cb(struct bufferevent *bev, void *ctx);
void udp_read_cb(evutil_socket_t fd, short events, void *ctx);
void tcp_accept(evutil_socket_t listener, short event, void *ctx);

#endif