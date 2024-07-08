#include "udp_cli_trigger.h"

const bool UdpCliTrigger::isRunning() const
{
    return running_;
}

std::uint16_t UdpCliTrigger::getId() const
{
    return id_;
}

const UdpCli &UdpCliTrigger::getUdpCli() const
{
    return cli_;
}
