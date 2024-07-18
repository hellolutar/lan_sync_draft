#include "net_framework_engine_base_event.h"

#include <cerrno>
#include <cstring>

#include <event2/bufferevent.h>
#include <event2/thread.h>

#include <netinet/tcp.h>

#include "recv_base_event.h"
#include "log/log.h"

void NetFrameworkEngineBaseEvent::init_check()
{
    if (!init_)
    {
        int ret = evthread_use_pthreads();
        if (ret != 0)
            printf("unsupport evthread_use_pthreads()\n");
        else
            init_ = true;
    }
}

std::shared_ptr<TcpServer> NetFrameworkEngineBaseEvent::addTcpServer(const NetAddr &addr)
{
    init_check();

    int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock < 0)
    {
        ERROR("NetFrameworkEngineBaseEvent::addTcpServer :", strerror(errno));
        shutdown();
        return nullptr;
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
        ERROR("NetFrameworkEngineBaseEvent::addTcpServer :", strerror(errno));
        shutdown();
        return nullptr;
    }

    int res = listen(tcp_sock, 100);
    if (res == -1)
    {
        ERROR("NetFrameworkEngineBaseEvent::addTcpServer :", strerror(errno));
        shutdown();
        return nullptr;
    }
    // todo 是否需要保存tcp_sock

    INFO("TCP listen: ", addr.str());
    auto dto = new BaseEngineDto(base_, shared_from_this());
    auto accept_event_persist = event_new(base_->getBase(), tcp_sock, EV_READ | EV_PERSIST, tcp_accept, dto);
    event_add(accept_event_persist, nullptr);

    auto srv = std::make_shared<TcpServer>(addr);
    addConn(srv);

    return srv;
}

std::shared_ptr<UdpServer> NetFrameworkEngineBaseEvent::addUdpServer(const NetAddr &addr)
{
    init_check();

    int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock <= 0)
    {
        ERROR("NetFrameworkEngineBaseEvent::addUdpServer :", strerror(errno));
        shutdown();
        return nullptr;
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
        ERROR("NetFrameworkEngineBaseEvent::addUdpServer :", strerror(errno));
        shutdown();
        return nullptr;
    }

    NetAddr addr_will_be_udpate_follow;

    auto srv = std::make_shared<UdpServer>(addr);

    auto dto = new UdpCbDto(srv, udp_sock);

    auto read_e = event_new(base_->getBase(), udp_sock, EV_READ | EV_PERSIST, udp_read_cb, dto);

    event_add(read_e, nullptr);

    addConn(srv);

    return srv;
}

std::shared_ptr<TcpCli> NetFrameworkEngineBaseEvent::connectWithTcp(const NetAddr &peer)
{
    init_check();

    INFO("TCP connect : ", peer.str());

    int peer_sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in be_addr = peer.sockaddrV4();
    int ret = connect(peer_sock, (struct sockaddr *)&be_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        ERROR("NetFrameworkEngineBaseEvent::connectWithTcp :", peer.str(), strerror(errno));
        return nullptr;
    }

    auto bevp = bufferevent_socket_new(base_->getBase(), peer_sock, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    if (bevp == nullptr)
    {
        ERROR("NetFrameworkEngineBaseEvent::connectWithTcp: bufferevent_socket_new has a error:", strerror(errno));
        exit(-1);
    }

    auto cli = std::make_shared<TcpCli>(peer);

    std::shared_ptr<BuffereventWrap> buf = std::make_shared<BuffereventWrap>(bevp);
    auto os = std::make_shared<OutputstreamBaseEvent>(buf);
    cli->setOutputStream(os);

    auto dto = new TcpCbDto(peer, cli);
    bufferevent_setcb(bevp, read_cb, write_cb, event_cb, dto);
    bufferevent_enable(bevp, EV_READ | EV_WRITE);
    bevp = nullptr;

    addConn(cli);

    return cli;
}

std::shared_ptr<UdpCli> NetFrameworkEngineBaseEvent::connectWithUdp(const NetAddr &peer)
{
    init_check();

    int peer_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (peer_sock <= 0)
    {
        ERROR("NetFrameworkEngineBaseEvent::connectWithUdp :", peer.str(), strerror(errno));
        return nullptr;
    }

    int optval = 1; // 这个值一定要设置，否则可能导致sendto()失败
    setsockopt(peer_sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
    setsockopt(peer_sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int));
    setsockopt(peer_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    evutil_make_socket_nonblocking(peer_sock);

    INFO("UDP connect :", peer.str().data());

    auto cli = std::make_shared<UdpCli>(peer);
    auto dto = new UdpCbDto(cli, peer_sock);

    auto read_e = event_new(base_->getBase(), peer_sock, EV_READ | EV_PERSIST, udp_read_cb, dto);
    event_add(read_e, nullptr);

    addConn(cli);

    return cli;
}

void NetFrameworkEngineBaseEvent::addConn(std::shared_ptr<NetAbility> ne)
{
    // todo
}

void NetFrameworkEngineBaseEvent::unregisterUdpCli(const NetAddr &addr)
{
}

std::shared_ptr<TcpCli> NetFrameworkEngineBaseEvent::findTcpCli(const NetAddr &addr)
{
    return nullptr;
}

void NetFrameworkEngineBaseEvent::start()
{
    event_base_dispatch(base_->getBase());
}

void NetFrameworkEngineBaseEvent::shutdown()
{
    event_base_loopbreak(base_->getBase());
}
