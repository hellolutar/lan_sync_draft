#include "network_adapter.h"

#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include "log/log.h"

using namespace std;

std::vector<NetAddr> NetworkAdapter::query_local_ports()
{
    vector<NetAddr> ports;

    int fd;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        close(fd);
        return {};
    }

    struct ifreq ifreqs[10];

    struct ifconf ifc;
    ifc.ifc_len = sizeof(ifreqs);
    ifc.ifc_buf = (caddr_t)ifreqs;

    if (ioctl(fd, SIOCGIFCONF, (char *)&ifc) < 0)
    {
        close(fd);
        return {};
    }

    uint16_t interface_num = ifc.ifc_len / sizeof(struct ifreq);

    for (size_t i = 0; i < interface_num; i++)
    {

        auto broad_addr = *(struct sockaddr_in *)&(ifreqs[i].ifr_broadaddr);
        if (broad_addr.sin_addr.s_addr == 0)
            continue; // is loopback

        // // 获取ip，并将其存储到ifreq中
        if (ioctl(fd, SIOCGIFADDR, &ifreqs[i]) < 0)
            continue;

        auto if_addr = *(struct sockaddr_in *)&(ifreqs[i].ifr_addr);

        NetAddr addr(if_addr, TransportType::UDP);

        ports.push_back(addr);
    }

    close(fd);

    return ports;
}

std::vector<NetAddr> NetworkAdapter::query_broad_ports()
{
    vector<NetAddr> ports;

    int fd;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        close(fd);
        return {};
    }

    struct ifreq ifreqs[10];

    struct ifconf ifc;
    ifc.ifc_len = sizeof(ifreqs);
    ifc.ifc_buf = (caddr_t)ifreqs;

    if (ioctl(fd, SIOCGIFCONF, (char *)&ifc) < 0)
    {
        close(fd);
        return {};
    }

    uint16_t interface_num = ifc.ifc_len / sizeof(struct ifreq);

    for (size_t i = 0; i < interface_num; i++)
    {

        auto broad_addr = *(struct sockaddr_in *)&(ifreqs[i].ifr_broadaddr);
        if (broad_addr.sin_addr.s_addr == 0)
            continue; // is loopback

        NetAddr addr(broad_addr, TransportType::UDP);

        ports.push_back(addr);
    }

    close(fd);

    return ports;
}

const std::optional<std::shared_ptr<ProtoSession>> NetworkAdapter::findSession(const NetAddr &peer)
{
    for (auto &&s : srvs_)
    {
        auto session_opt = s->findSession(peer);
        if (session_opt.has_value())
            return session_opt;
    }
    return nullopt;
}

void NetworkAdapter::start(std::shared_ptr<LogicWrite> core)
{
    auto ports = query_local_ports();
    for (auto &&p : ports)
    {
        srvs_.push_back(std::make_shared<ProtoServer>(p, core));
    }
}

void NetworkAdapter::write(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    if (peer.type() == TransportType::TCP)
    {
        auto session = findSession(peer);
        if (session.has_value())
            session.value()->write(data, size);
        else
            WARN_F("NetworkAdapter::write() ", " can't send msg, because can not found session for {} ", peer.str());
    }
    else
    {
        udp_write(peer, data, size);
    }
}

void NetworkAdapter::udp_write(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    auto eg = Netframework::getEngine();
    auto u_cli = eg->connectWithUdp(peer);
    u_cli->write(data, size);
}
void NetworkAdapter::rmSessionCallBack(const NetAddr &addr)
{
    for (auto &&s : srvs_)
    {
        if (s->rmSessionIfExist(addr))
            return;
    }
};
