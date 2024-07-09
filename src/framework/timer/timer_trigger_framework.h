#ifndef __TIMER_TRIGGER_FRAMEWORK_H_
#define __TIMER_TRIGGER_FRAMEWORK_H_

#include <memory>

#include "timer_trigger.h"

class TriggerWithEvent;

class TimerFrameworkEngine
{
protected:
    std::map<Trigger *, Trigger *> trgs_;

public:
    TimerFrameworkEngine(/* args */) {}
    ~TimerFrameworkEngine() {}

    virtual bool addTrg(TriggerWithEvent *tr);
    virtual bool delTrg(Trigger *tr);
    virtual void run();
};

class TimerFramework : public Timer
{
public:
    virtual ~TimerFramework(){};

    static void init(std::shared_ptr<TimerFrameworkEngine> eg);
    static std::shared_ptr<TimerFrameworkEngine> getEngine();
};

#endif