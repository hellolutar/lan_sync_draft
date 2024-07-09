#include "network_context.h"

void NetworkContext::write(uint8_t *data, uint64_t size)
{
    static_cast<const NetworkContext &>(*this).write(data, size);
}

void NetworkContext::write(uint8_t *data, uint64_t size) const
{
    na_->write(road_id_, data, size);
}

bool NetworkContext::operator==(const NetworkContext &other) const
{
    return road_id_ == other.road_id_;
}
