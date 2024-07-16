#include "network_adapter.h"

std::vector<NetAddr> NetworkAdapter::query_local_ports()
{
    auto addr = NetAddr("0.0.0.10:8080");
    auto addr2 = NetAddr("0.0.0.20:8080");
    return {addr, addr2};
}

const ProtoSession &NetworkAdapter::findSession(const NetAddr &peer)
{
    for (auto &&s : srvs_)
    {
        try
        {
            auto &session = s->findSession(peer);
            return session;
        }
        catch (const std::exception &e)
        {
        }
    }
    throw;
}

void NetworkAdapter::start(std::shared_ptr<Logic> core)
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
        auto &session = findSession(peer);
        session.write(data, size);
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
    // eg->unregisterUdpCli(u_cli->peer());
};
