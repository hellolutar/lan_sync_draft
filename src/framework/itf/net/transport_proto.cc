#include "transport_proto.h"
#include "log/log.h"

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
    if (os_ != nullptr)
        os_->write(data, size);
    else
        logic_->write(data, size);
}

void NetAbility::setOutputStream(std::unique_ptr<OutputStream> &&os)
{
    os_ = std::move(os);
}

void NetAbility::bind(std::shared_ptr<LogicWrite> logic)
{
    logic_ = logic;
    logic_->setOutputStream(std::move(os_));
    os_ = nullptr;
}

const std::unique_ptr<OutputStream> &NetAbility::getOutputStream()
{
    if (os_ != nullptr)
        return os_;
    return logic_->getOutputStream();
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

UdpCli::~UdpCli() {
    DEBUG("UdpCli::~UdpCli(): destroy (", peer_.str(), ")");
}

void UdpCli::write(std::shared_ptr<uint8_t[]> data, uint64_t size) {
  auto peer_addr = peer_.sockaddrV4();
  sendto(sock_, data.get(), size, 0, reinterpret_cast<sockaddr *>(&peer_addr),
         sizeof(sockaddr_in));
}

TcpCli::~TcpCli()
{
    logic_ = nullptr;
    DEBUG("TcpCli::~TcpCli(): destroy (", peer_.str(), ")");
}