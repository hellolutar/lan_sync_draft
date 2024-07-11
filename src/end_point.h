#ifndef __END_POINT_H_
#define __END_POINT_H_

#include "logic/logic_core.h"
#include "task/task_coordinator.h"
#include "framework/timer/timer_trigger_framework.h"
#include "framework/net/net_framework.h"
#include "udp_cli_trigger.h"

class Endpoint
{
private:
    void registerTrg();

protected:
    std::shared_ptr<NetworkAdapter> na_;
    std::shared_ptr<LogicCore> core_;
    std::shared_ptr<UdpCliTrigger> trg_;

public:
    Endpoint()
        : na_(std::make_shared<NetworkAdapter>()), core_(std::make_shared<LogicCore>())
    {
        core_->setNetworkAdapter(na_);
    };

    virtual ~Endpoint() {}

    void run();
};

#endif