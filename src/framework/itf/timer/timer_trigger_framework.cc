#include "framework/itf/timer/timer_trigger_framework.h"
#include "timer_trigger_framework.h"

bool TimerFrameworkEngine::addTrg(std::shared_ptr<Trigger> tr)
{
    if (trgs_.find(tr) == trgs_.end())
    {
        tr->setId(incr_++);
        trgs_.insert(tr);
        return true;
    }
    return false;
}

bool TimerFrameworkEngine::delTrg(std::shared_ptr<Trigger> tr)
{
    auto iter = trgs_.find(tr);
    if (iter != trgs_.end())
    {
        trgs_.erase(iter);
        return true;
    }
    return false;
}

std::shared_ptr<TimerFrameworkEngine> TimerFramework::engine_;

void TimerFramework::init(std::shared_ptr<TimerFrameworkEngine> eg)
{
    engine_ = eg;
}

std::shared_ptr<TimerFrameworkEngine> TimerFramework::getEngine()
{
    return engine_;
}
