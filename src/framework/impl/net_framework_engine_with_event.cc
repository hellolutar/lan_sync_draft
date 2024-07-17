#include "net_framework_engine_with_event.h"

#include <cassert>

class BaseEngineDto
{
private:
    std::shared_ptr<event_base> base_;
    std::shared_ptr<NetframeworkEngine> engine_;

public:
    BaseEngineDto(std::shared_ptr<event_base> base, std::shared_ptr<NetframeworkEngine> engine)
        : base_(base), engine_(engine) {}
    ~BaseEngineDto()
    {
        base_ = nullptr;
        engine_ = nullptr;
    }

    std::shared_ptr<event_base> getBase()
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

void event_cb(struct bufferevent *bev, short events, void *ctx)
{
    switch (events)
    {
    case BEV_EVENT_EOF:
        // LOG_ERROR("NetFrameworkImplWithEvent::event_cb : BEV_EVENT_EOF : {}", strerror(errno));
        break;
    case BEV_EVENT_ERROR:
        // LOG_ERROR("NetFrameworkImplWithEvent::event_cb : BEV_EVENT_ERROR : {}", strerror(errno));
        break;
    case BEV_EVENT_TIMEOUT:
        // LOG_ERROR("NetFrameworkImplWithEvent::event_cb : BEV_EVENT_TIMEOUT : {}", strerror(errno));
        break;
    case BEV_EVENT_CONNECTED:
        // LOG_ERROR("NetFrameworkImplWithEvent::event_cb : BEV_EVENT_CONNECTED : {}", strerror(errno));
        break;
    case BEV_EVENT_READING:
        // LOG_ERROR("NetFrameworkImplWithEvent::event_cb : BEV_EVENT_READING : {}", strerror(errno));
        break;
    case BEV_EVENT_WRITING:
        // LOG_ERROR("NetFrameworkImplWithEvent::event_cb : BEV_EVENT_WRITING : {}", strerror(errno));
        break;
    default:
        // LOG_ERROR("NetFrameworkImplWithEvent::event_cb : OTHER : {}", strerror(errno));
        break;
    }
}

void write_cb(struct bufferevent *bev, void *data)
{
}

void read_cb(struct bufferevent *bev, void *ctx)
{
    auto dto = reinterpret_cast<TcpCbDto *>(ctx);
    auto ne = dto->getNe();
    auto peer = dto->getPeer();
    delete dto;

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
        uint8_t *data = head.get();
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
            // LOG_WARN("NetFrameworkImplWithEvent::read_cb : READ TIMES must <= LIMIT({})", limit);
            break;
        }
    }

    head = nullptr;
}

void udp_read_cb(evutil_socket_t fd, short events, void *ctx)
{
    struct sockaddr_in target_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    char data[4096] = {0};

    uint32_t receive = recvfrom(fd, data, 4096, 0, (sockaddr *)&target_addr, &addrlen);
    if (receive <= 0)
    {
        // LOG_WARN("[SYNC SER] cannot receive anything !");
        return;
    }
    // std::shared_ptr<NetworkConnCtx> nctx(static_cast<NetworkConnCtx *>(arg));

    // nctx->setNetAddr(NetAddr::fromBe(target_addr));

    // NetAbility *ne = nctx->getNetworkEndpoint();

    // ne->recv((void *)data, receive, nctx);
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

    auto bevp = bufferevent_socket_new(base.get(), peer_sock, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    if (bevp == nullptr)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::tcp_accept(): bufferevent_socket_new has a error: {}", strerror(errno));
        delete dto;
        exit(-1);
    }

    // todo(20240717, lutar) set outputstream

    NetAddr peer(peer_addr);
    auto cli = std::make_shared<TcpCli>(peer);
    engine->addConn(cli);

    uint64_t hw = 0;
    uint64_t lw = 0;
    bufferevent_getwatermark(bevp, EV_WRITE, &lw, &hw);
    bufferevent_setwatermark(bevp, EV_WRITE, 0, hw);

    auto cbDto = new TcpCbDto(peer, cli);

    bufferevent_setcb(bevp, read_cb, write_cb, event_cb, cbDto);
    bufferevent_enable(bevp, EV_READ | EV_WRITE);
}

void NetFrameworkEngineBaseEvent::init_check()
{
}

std::shared_ptr<TcpServer> NetFrameworkEngineBaseEvent::addTcpServer(const NetAddr &addr)
{
    int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock < 0)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::addTcpServer : {}", strerror(errno));
        shutdown();
        return;
    }

    evutil_make_socket_nonblocking(tcp_sock);
    int optval = 1;
    setsockopt(tcp_sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
    setsockopt(tcp_sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int));
    setsockopt(tcp_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    setsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int));

    sockaddr_in be_addr = addr.sockaddrV4();

    if (bind(tcp_sock, (struct sockaddr *)&be_addr, sizeof(struct sockaddr_in)) != 0)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::addTcpServer : {}", strerror(errno));
        shutdown();
        return;
    }

    int res = listen(tcp_sock, 100);
    if (res == -1)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::addTcpServer : {}", strerror(errno));
        shutdown();
        return;
    }
    // todo 是否需要保存tcp_sock

    // LOG_INFO("TCP listen: {}", ne->getAddr().str().data());
    auto dto = new BaseEngineDto(base_, shared_from_this());
    auto accept_event_persist = event_new(base_.get(), tcp_sock, EV_READ | EV_PERSIST, tcp_accept, dto);
    event_add(accept_event_persist, nullptr);

    auto srv = std::make_shared<TcpServer>(addr);
    addConn(srv);

    return srv;
}

std::shared_ptr<UdpServer> NetFrameworkEngineBaseEvent::addUdpServer(const NetAddr &addr)
{
    int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock <= 0)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::addUdpServer : {}", strerror(errno));
        shutdown();
        return;
    }

    int optval = 1;
    setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
    setsockopt(udp_sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int));
    setsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    evutil_make_socket_nonblocking(udp_sock);

    auto be_addr = addr.sockaddrV4();

    int udpret = bind(udp_sock, (struct sockaddr *)&be_addr, sizeof(struct sockaddr_in));
    if (udpret != 0)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::addUdpServer : {}", strerror(errno));
        shutdown();
        return;
    }

    auto srv = std::make_shared<UdpServer>(addr);
    addConn(srv);
    // todo(20240717, lutar) set outputstream

    NetAddr addr_will_be_udpate_follow;

    auto read_e = event_new(base_.get(), udp_sock, EV_READ | EV_PERSIST, udp_read_cb, base_.get());

    event_add(read_e, nullptr);

    return srv;
}

std::shared_ptr<TcpCli> NetFrameworkEngineBaseEvent::connectWithTcp(const NetAddr &peer)
{
    // LOG_INFO("TCP connect : {}", peer_ne->getAddr().str());

    int peer_sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in be_addr = peer.sockaddrV4();
    int ret = connect(peer_sock, (struct sockaddr *)&be_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::connectWithTcp {} : {}", peer_ne->getAddr().str(), strerror(errno));
        // delete peer_ne;
        return nullptr;
    }

    struct bufferevent *bev = bufferevent_socket_new(base_.get(), peer_sock, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    if (bev == nullptr)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::tcp_accept(): bufferevent_socket_new has a error: {}", strerror(errno));
        exit(-1);
    }

    // todo(20240717, lutar) set outputstream

    auto cli = std::make_shared<TcpCli>(peer);
    auto dto = new TcpCbDto(peer, cli);
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, dto);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    addConn(cli);

    return cli;
}

std::shared_ptr<UdpCli> NetFrameworkEngineBaseEvent::connectWithUdp(const NetAddr &peer)
{
    int peer_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (peer_sock <= 0)
    {
        // LOG_ERROR("NetFrameworkImplWithEvent::connectWithUdp {} : {}", peer_ne->getAddr().str(), strerror(errno));
        // delete peer_ne;
        return nullptr;
    }

    int optval = 1; // 这个值一定要设置，否则可能导致sendto()失败
    setsockopt(peer_sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
    setsockopt(peer_sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int));
    setsockopt(peer_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    evutil_make_socket_nonblocking(peer_sock);

    // LOG_INFO("UDP connect : {}", peer_ne->getAddr().str().data());

    // std::shared_ptr<NetworkConnCtxWithEventForUDP> nctx = make_shared<NetworkConnCtxWithEventForUDP>(peer_ne, peer_sock, peer_ne->getAddr()); // todo &peer_ne maybe a problem

    struct event *read_e = event_new(base_.get(), peer_sock, EV_READ | EV_PERSIST, udp_read_cb, base_.get());
    event_add(read_e, nullptr);
    // peer_ne->setEvent(read_e);
}

void NetFrameworkEngineBaseEvent::unregisterUdpCli(const NetAddr &addr)
{
}

std::shared_ptr<TcpCli> NetFrameworkEngineBaseEvent::findTcpCli(const NetAddr &addr)
{
}

void NetFrameworkEngineBaseEvent::run()
{
    event_base_dispatch(base_.get());
}

void NetFrameworkEngineBaseEvent::shutdown()
{
    event_base_loopbreak(base_.get());
}
