#include "recv_base_event.h"

#include <cstring>
#include <cassert>

#include <event2/buffer.h>
#include <event2/event.h>
// #include <event2/bufferevent.h>

#include "log/log.h"

void event_cb(struct bufferevent *bev, short events, void *ctx)
{
    switch (events)
    {
    case BEV_EVENT_EOF:
        // LOG_ERROR("NetFrameworkEngineBaseEvent::event_cb : BEV_EVENT_EOF : {}", strerror(errno));
        break;
    case BEV_EVENT_ERROR:
        // LOG_ERROR("NetFrameworkEngineBaseEvent::event_cb : BEV_EVENT_ERROR : {}", strerror(errno));
        break;
    case BEV_EVENT_TIMEOUT:
        // LOG_ERROR("NetFrameworkEngineBaseEvent::event_cb : BEV_EVENT_TIMEOUT : {}", strerror(errno));
        break;
    case BEV_EVENT_CONNECTED:
        // LOG_ERROR("NetFrameworkEngineBaseEvent::event_cb : BEV_EVENT_CONNECTED : {}", strerror(errno));
        break;
    case BEV_EVENT_READING:
        // LOG_ERROR("NetFrameworkEngineBaseEvent::event_cb : BEV_EVENT_READING : {}", strerror(errno));
        break;
    case BEV_EVENT_WRITING:
        // LOG_ERROR("NetFrameworkEngineBaseEvent::event_cb : BEV_EVENT_WRITING : {}", strerror(errno));
        break;
    default:
        // LOG_ERROR("NetFrameworkEngineBaseEvent::event_cb : OTHER : {}", strerror(errno));
        break;
    }
}

void write_cb(struct bufferevent *bev, void *data)
{
}

void read_cb(struct bufferevent *bev, void *ctx)
{
    auto dto = reinterpret_cast<TcpConn *>(ctx);
    auto ne = dto->getNe();
    auto peer = dto->getPeer();

    struct evbuffer *in = bufferevent_get_input(bev);

    uint32_t recvLen = evbuffer_get_length(in);
    if (recvLen == 0)
        return;

    std::shared_ptr<uint8_t[]> head(new uint8_t[recvLen]());

    int limit = 1024;
    int i = 0;
    uint64_t ne_wanto_extra_len = 0;
    int actual_extra_len = 0;
    while (true)
    {
        recvLen = evbuffer_get_length(in);
        auto data = head.get();
        memset(data, 0, recvLen);
        evbuffer_copyout(in, data, recvLen);

        ne_wanto_extra_len = ne->isExtraAllDataNow(head, recvLen);

        if (ne_wanto_extra_len == 0 || ne_wanto_extra_len > recvLen)
            break;

        memset(data, 0, recvLen);
        actual_extra_len = evbuffer_remove(in, data, ne_wanto_extra_len);
        // LOG_DEBUG("buf_len:{} \t, want_extra_len:{}, actual_extra_len:{}\t remaind:{}", recvLen, ne_wanto_extra_len, actual_extra_len, evbuffer_get_length(in));

        assert(actual_extra_len == ne_wanto_extra_len);

        ne->recv(peer, head, actual_extra_len);
        if (i++ >= limit)
        {
            // LOG_WARN("NetFrameworkEngineBaseEvent::read_cb : READ TIMES must <= LIMIT({})", limit);
            break;
        }
    }

    head = nullptr;
}

void udp_read_cb(evutil_socket_t fd, short events, void *ctx)
{

    auto dto = reinterpret_cast<UdpConn *>(ctx);
    auto ne = dto->getNe();
    auto sock = dto->getSock();

    struct sockaddr_in target_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    std::shared_ptr<uint8_t[]> data(new uint8_t[4096]());

    uint32_t receive = recvfrom(fd, data.get(), 4096, 0, (sockaddr *)&target_addr, &addrlen);
    if (receive <= 0)
    {
        // LOG_WARN("[SYNC SER] cannot receive anything !");
        return;
    }

    NetAddr peer(target_addr);
    auto os = std::make_unique<OutputstreamForUdp>(peer, sock);
    ne->setOutputStream(std::move(os));

    ne->recv(peer, data, receive);
}

void tcp_accept(evutil_socket_t listener, short event, void *ctx)
{
    auto dto = reinterpret_cast<BaseEngineDto *>(ctx);
    auto engine = dto->getEngine();
    auto base = dto->getBase();
    delete dto;

    struct sockaddr_in peer_addr;
    socklen_t slen = sizeof(struct sockaddr_in);

    int peer_sock = accept(listener, (struct sockaddr *)&peer_addr, &slen);
    assert(peer_sock > 0 && peer_sock < FD_SETSIZE);

    evutil_make_socket_nonblocking(peer_sock);
    evutil_make_listen_socket_reuseable_port(peer_sock);

    auto bevp = bufferevent_socket_new(base->getBase(), peer_sock, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    if (bevp == nullptr)
    {
        ERROR("NetFrameworkEngineBaseEvent::tcp_accept(): bufferevent_socket_new has a error", strerror(errno));
        delete dto;
        exit(-1);
    }

    NetAddr peer(peer_addr);
    auto cli = std::make_shared<TcpCli>(peer);

    uint64_t hw = 0;
    uint64_t lw = 0;
    bufferevent_getwatermark(bevp, EV_WRITE, &lw, &hw);
    bufferevent_setwatermark(bevp, EV_WRITE, 0, hw);

    auto conn = std::make_shared<TcpConn>(peer, cli);
    auto event_wrap = std::make_shared<BuffereventWrap>(bevp);
    conn->setEvent(event_wrap);
    engine->addConn(conn);

    // todo conn->setEvent()

    bufferevent_setcb(bevp, read_cb, write_cb, event_cb, conn.get());
    bufferevent_enable(bevp, EV_READ | EV_WRITE);

    auto os = std::make_unique<OutputstreamBaseEvent>(event_wrap);
    cli->setOutputStream(std::move(os));
}