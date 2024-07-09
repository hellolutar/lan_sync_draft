#ifndef __TIMER_TRIGGER_FRAMEWORK_H_
#define __TIMER_TRIGGER_FRAMEWORK_H_

#include <set>
#include <memory>

#include "timer_trigger.h"

class TimerFrameworkEngine
{
protected:
    std::set<std::shared_ptr<Trigger>> trgs_;

public:
    TimerFrameworkEngine(/* args */) {}
    ~TimerFrameworkEngine() {}

    virtual bool addTrg(std::shared_ptr<Trigger> tr);
    virtual bool delTrg(std::shared_ptr<Trigger> tr);
    virtual void run() = 0;
};

class TimerFramework : public Timer
{
public:
    virtual ~TimerFramework() {};

    static void init(std::shared_ptr<TimerFrameworkEngine> eg);
    static std::shared_ptr<TimerFrameworkEngine> getEngine();
};

#endif