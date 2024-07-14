#include "network_context.h"

void NetworkContext::write(uint8_t *data, uint64_t size)
{
    static_cast<const NetworkContext &>(*this).write(data, size);
}

void NetworkContext::write(uint8_t *data, uint64_t size) const
{
    na_->write(peer_, data, size);
}

const NetAddr &NetworkContext::getPeer() const
{
    return peer_;
}

bool NetworkContext::operator==(const NetworkContext &other) const
{
    return peer_ == other.peer_;
}

bool NetworkContext::operator<(const NetworkContext &other) const
{
    return peer_ < other.peer_;
}
