#ifndef __UDP_CLI_TRIGGER_H_
#define __UDP_CLI_TRIGGER_H_

#include "network_context.h"
#include "buf/buf_base_on_event.h"
#include "framework/itf/net/transport_proto.h"
#include "framework/itf/net/net_framework.h"
#include "framework/itf/timer/timer_trigger.h"

class UdpCliTrigger : public Trigger
{
private:
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