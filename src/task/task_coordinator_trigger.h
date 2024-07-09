#ifndef __TASK_COORDINATOR_TRIGGER_H_
#define __TASK_COORDINATOR_TRIGGER_H_

#include "framework/timer/timer_trigger.h"
#include "task/task_coordinator.h"

class TaskCoordinatorTrigger : public Trigger
{
private:
    std::chrono::_V2::system_clock::time_point last_;
    TaskCoordinator &coor_;

public:
    TaskCoordinatorTrigger(struct timeval period, bool persist, TaskCoordinator &coor)
        : coor_(coor), Trigger(period, persist), last_(std::chrono::system_clock::now()){};

    ~TaskCoordinatorTrigger(){};

    void trigger() override;

    TaskCoordinator &getTaskCoordinator();
};

#endif