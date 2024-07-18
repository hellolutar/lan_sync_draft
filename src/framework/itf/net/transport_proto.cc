#include "transport_proto.h"

const uint64_t NetAbility::isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    return logic_->isExtraAllDataNow(data, size);
}

void NetAbility::recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    logic_->recv(peer, data, size);
}

void NetAbility::write(std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    os_->write(data, size);
}

void NetAbility::setOutputStream(std::shared_ptr<OutputStream> os)
{
    os_ = os;
}

void NetAbility::bind(std::shared_ptr<LogicWrite> logic)
{
    logic_ = logic;
    logic_->setOutputStream(os_);
}

std::shared_ptr<OutputStream> NetAbility::getOutputStream()
{
    return os_;
}

void OutputStream::close()
{
    closed_ = true;
}

bool OutputStream::isClosed()
{
    return closed_;
}

const NetAddr &ConnCli::peer() const
{
    return peer_;
}
