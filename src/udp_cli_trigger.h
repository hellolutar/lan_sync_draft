#ifndef __UDP_CLI_TRIGGER_H_
#define __UDP_CLI_TRIGGER_H_

#include <cstdint>

#include "net/transport_proto.h"


class UdpCliTrigger
{
private:
    std::uint16_t id_;
    std::uint32_t road_id_;
    
    UdpCli cli_;
    bool running_ = false;
    
public:
    UdpCliTrigger(/* args */) {}
    ~UdpCliTrigger() {}
    const bool isRunning() const;
    std::uint16_t getId() const;

    const UdpCli& getUdpCli() const;
};


#endif