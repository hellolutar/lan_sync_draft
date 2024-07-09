#include "proto_session_manager.h"

const uint64_t ProtoSessionManagerLogic::isExtraAllDataNow(uint8_t *data, uint64_t data_len) const
{
    return data_len >= LEN_LAN_SYNC_HEADER_T;
}

const uint16_t ProtoSessionManagerLogic::genRoadId()
{
    // todo 目前的用法不正确
    incr_.store(incr_.load() + 1);
    return session_id_ << 16 | incr_.load();
}

void ProtoSessionManagerLogic::recv(const NetAddr &from, uint8_t *data, uint64_t size)
{
    for (auto &&r : *roads_)
    {
        if (r->isMineRoad(from))
        {
            r->recv(from, data, size);
            return;
        }
    }
    // auto session_recv_f = std::bind(&ProtoSessionManager::recv, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    auto r = std::make_shared<RoadMaintainer>(genRoadId(), nullptr, nullptr);
    r->recv(from, data, size);
    roads_->push_back(std::move(r));
}

const uint16_t ProtoSessionManager::getId() const
{
    return id_;
}

uint32_t ProtoSessionManager::connPeer(const NetAddr &peer)
{
}

std::shared_ptr<RoadMaintainer> ProtoSessionManager::findRoad(uint32_t road_id) const
{
    for (auto &&r : roads_)
    {
        if (r->getId() == road_id)
            return r;
    }
    throw "RoadMaintainer not found";
}

void ProtoSessionManager::write(uint32_t road_id, uint8_t *data, uint64_t size) const
{
    auto r = findRoad(road_id);
    r->tcp_write(data, size);
}
