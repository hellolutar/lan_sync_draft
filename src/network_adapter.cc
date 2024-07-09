#include "network_adapter.h"

std::vector<NetAddr> NetworkAdapter::query_local_ports()
{
    auto addr = NetAddr("192.168.233.1:8080");
    auto addr2 = NetAddr("10.0.0.1:8080");
    return {addr, addr2};
}

const std::shared_ptr<ProtoSessionManager>NetworkAdapter::findSession(uint32_t road_id)
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

void NetworkAdapter::setLogicRecvFunc(std::function<void(uint32_t, uint8_t *, uint64_t)> f)
{
    logic_recv_f_ = f;
}

void NetworkAdapter::start()
{
    auto ports = query_local_ports();
    for (auto &&p : ports)
    {
        sess_.push_back(std::make_shared<ProtoSessionManager>(logic_recv_f_, p));
    }
}

uint32_t NetworkAdapter::connPeer(const NetAddr &p)
{
    sess_.push_back(std::make_shared<ProtoSessionManager>(logic_recv_f_, p));

    // 随便找一个session,然后创建udpcli
    for (auto &&session : sess_)
    {
        session->
    }
    
}

void NetworkAdapter::write(uint32_t road_id, uint8_t *data, uint64_t size)
{
    auto session = findSession(road_id);
    session->write(road_id, data, size);
}
