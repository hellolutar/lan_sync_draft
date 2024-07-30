#ifndef __TIMER_TRIGGER_FRAMEWORK_H_
#define __TIMER_TRIGGER_FRAMEWORK_H_

#include <set>
#include <memory>

#include "framework/itf/timer/timer_trigger.h"

class TimerFrameworkEngine
{
protected:
    std::set<std::shared_ptr<Trigger>> trgs_;
    uint16_t incr_ = 0;

public:
    TimerFrameworkEngine(/* args */) {}
    virtual ~TimerFrameworkEngine()
    {
        trgs_.clear();
    }

    virtual bool addTrg(std::shared_ptr<Trigger> tr);
    virtual bool delTrg(std::shared_ptr<Trigger> tr);
    virtual void run() = 0;
    virtual void shutdown() {};
};

class TimerFramework : public Timer
{
protected:
    static std::shared_ptr<TimerFrameworkEngine> engine_;

public:
    virtual ~TimerFramework(){};

    static void init(std::shared_ptr<TimerFrameworkEngine> eg);
    static std::shared_ptr<TimerFrameworkEngine> getEngine();
};

#endif