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
    std::vector<NetworkContext> ctxs_;
    bool running_ = false;

public:
    UdpCliTrigger(const NetworkContext &ctx, struct timeval period, bool persist)
        : Trigger(period, persist)
    {
        ctxs_.push_back(ctx);
    }
    ~UdpCliTrigger() {}

    void trigger() override;

    const bool isRunning() const;
    std::uint16_t getId() const;

    void addCtx(const NetworkContext &ctx);
    void delCtx(const NetAddr& addr);
    uint64_t ctxSize();
};

#endif