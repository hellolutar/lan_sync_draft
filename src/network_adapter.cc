#include "network_adapter.h"

std::vector<NetAddr> NetworkAdapter::query_local_ports()
{
    auto addr = NetAddr("192.168.233.1:8080");
    auto addr2 = NetAddr("10.0.0.1:8080");
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

void NetworkAdapter::write(const NetAddr &peer, uint8_t *data, uint64_t size)
{
    auto &session = findSession(peer);
    session.write(data, size);
}
