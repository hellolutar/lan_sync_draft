#ifndef __UDP_CLI_TRIGGER_H_
#define __UDP_CLI_TRIGGER_H_

#include <cstdint>

#include "network_context.h"
#include "buf/buf_base_on_event.h"
#include "framework/net/transport_proto.h"
#include "framework/net/net_framework.h"

class UdpCliTrigger : public Trigger
{
private:
    std::uint16_t id_;

    NetworkContext ctx_;
    bool running_ = false;

public:
    UdpCliTrigger(NetworkContext ctx, struct timeval period, bool persist)
        : Trigger(period, persist), ctx_(ctx) {}
    ~UdpCliTrigger() {}

    void trigger() override;

    const bool isRunning() const;
    std::uint16_t getId() const;
};

#endif