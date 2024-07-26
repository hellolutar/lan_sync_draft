#include "transport_proto.h"
#include "log/log.h"

const uint64_t NetAbility::isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    return logic_->isExtraAllDataNow(data, size);
}

void NetAbility::recv(NetAbilityContext &ctx, std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    logic_->recv(ctx, data, size);
}

void NetAbility::bind(std::shared_ptr<LogicWrite> logic)
{
    // todo(lutar,20240726) 这里的代码如此写，非常不合理
    if (logic_ != nullptr && logic_->getOutputStream() != nullptr)
        return;

    logic_ = logic;
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

void ConnCli::write(std::shared_ptr<uint8_t[]> data, uint64_t size) {
    os_->write(data,size);
}

void ConnCli::setOutputStream(std::shared_ptr<OutputStream> os)
{
    os_ = os;
}

std::shared_ptr<OutputStream> ConnCli::os()
{
    return os_;
}

UdpCli::~UdpCli()
{
    DEBUG("UdpCli::~UdpCli(): destroy (", peer_.str(), ")");
}

TcpCli::~TcpCli()
{
    logic_ = nullptr;
    DEBUG("TcpCli::~TcpCli(): destroy (", peer_.str(), ")");
}

TcpServer::~TcpServer()
{
    logic_ = nullptr;
    DEBUG("TcpServer::~TcpServer(): destroy (", port_.str(), ")");
}

const std::shared_ptr<LogicWrite> TcpServer::getLogic() const
{
    return logic_;
}

UdpServer::~UdpServer()
{
    logic_ = nullptr;
    DEBUG("UdpServer::~UdpServer(): destroy (", port_.str(), ")");
}

void NetAbilityContext::write(std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    os_->write(data, size);
}

const NetAddr &NetAbilityContext::from() const
{
    return from_;
}
