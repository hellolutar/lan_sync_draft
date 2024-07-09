#ifndef __UDP_CLI_TRIGGER_H_
#define __UDP_CLI_TRIGGER_H_

#include <cstdint>

#include "framework/net/transport_proto.h"
#include "framework/net/net_framework.h"

class UdpCliTrigger
{
private:
    std::uint16_t id_;
    std::uint32_t road_id_;

    std::shared_ptr<UdpCli> cli_;
    bool running_ = false;

public:
    UdpCliTrigger(){};
    UdpCliTrigger(std::uint32_t road_id, const NetAddr &peer) : road_id_(road_id)
    {
        auto eg = Netframework::getEngine();
        cli_ = eg->connectWithUdp(peer);
    }
    ~UdpCliTrigger() {}
    const bool isRunning() const;
    std::uint16_t getId() const;

    const std::shared_ptr<UdpCli> getUdpCli() const;
};

#endif