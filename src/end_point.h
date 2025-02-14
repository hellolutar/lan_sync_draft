#ifndef __END_POINT_H_
#define __END_POINT_H_

#include "framework/itf/net/net_framework.h"
#include "framework/itf/timer/timer_trigger_framework.h"
#include "logic/logic_core.h"
#include "task/task_coordinator_trigger.h"
#include "udp_cli_trigger.h"

class Endpoint
{
private:
    void registerTrg();

protected:
    std::shared_ptr<NetworkAdapter> na_;
    std::shared_ptr<LogicCore> core_;
    std::shared_ptr<UdpCliTrigger> hello_trg_;
    std::shared_ptr<TaskCoordinatorTrigger> coor_trg_;
    std::shared_ptr<TaskManager> tm_;
    std::shared_ptr<ResourceManager> rm_;
    std::shared_ptr<PropertiesParse> conf_;

public:
    Endpoint() {}
    virtual ~Endpoint() {}

    virtual void init();
    virtual void run();

    void setTaskManager(std::shared_ptr<TaskManager> tm);
    void setNetworkAdapter(std::shared_ptr<NetworkAdapter> tm);
    void setResourceManager(std::shared_ptr<ResourceManager> rm);
    void setPropertiesParse(std::shared_ptr<PropertiesParse> conf);

    std::shared_ptr<UdpCliTrigger> getUdpCliTrigger();
    
};

#endif