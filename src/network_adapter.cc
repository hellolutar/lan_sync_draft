#include "network_adapter.h"

std::vector<NetAddr> NetworkAdapter::query_local_ports()
{
    auto addr = NetAddr("192.168.233.1:8080");
    auto addr2 = NetAddr("10.0.0.1:8080");
    return {addr, addr2};
}

const std::shared_ptr<ProtoSession> NetworkAdapter::findSession(uint32_t road_id)
{
    // todo
    uint16_t session_id = road_id >> 16;
    for (auto &&r : sess_)
    {
        if (r->getId() == session_id)
            return r;
    }

    throw "ProtoSessionManager not found";
}

void NetworkAdapter::start()
{
    auto ports = query_local_ports();
    for (auto &&p : ports)
    {
        sess_.push_back(std::make_shared<ProtoSession>(p));
    }
}

uint32_t NetworkAdapter::setUpSessionWithPeer(const NetAddr &p)
{
    sess_.push_back(std::make_shared<ProtoSession>(p));

    // 随便找一个session,然后创建udpcli
    for (auto &&session : sess_)
    {
        session->
    }
}

void NetworkAdapter::write(uint32_t road_id, uint8_t *data, uint64_t size)
{
    auto session = findSession(road_id);
    session->write( data, size);
}
