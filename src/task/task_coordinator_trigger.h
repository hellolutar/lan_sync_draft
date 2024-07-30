#ifndef __TASK_COORDINATOR_TRIGGER_H_
#define __TASK_COORDINATOR_TRIGGER_H_

#include "framework/itf/timer/timer_trigger.h"
#include "task/task_coordinator.h"

class TaskCoordinatorTrigger : public Trigger
{
private:
    std::chrono::_V2::system_clock::time_point last_;
    std::shared_ptr<TaskCoordinator> coor_;

public:
    TaskCoordinatorTrigger(struct timeval period, bool persist, std::shared_ptr<TaskCoordinator> coor)
        : coor_(coor), Trigger(period, persist), last_(std::chrono::system_clock::now()){};

    ~TaskCoordinatorTrigger(){};

    void trigger() override;

    std::shared_ptr<TaskCoordinator> getTaskCoordinator();
};

#endif