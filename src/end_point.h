#ifndef __END_POINT_H_
#define __END_POINT_H_

#include "logic_core.h"
#include "task/task_coordinator.h"
#include "framework/timer/timer_trigger_framework.h"

class Endpoint
{
protected:
    std::shared_ptr<NetworkAdapter> na_;
    LogicCore core_;

public:
    Endpoint() :na_(std::make_shared<NetworkAdapter>()), core_(na_)
    {
        auto f = bind(&LogicCore::recv, &core_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        na_->setLogicRecvFunc(f);
    };
    
    virtual ~Endpoint() {}

    void run();

};

#endif