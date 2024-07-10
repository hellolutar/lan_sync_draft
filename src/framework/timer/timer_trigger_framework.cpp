#include "framework/timer/timer_trigger_framework.h"

bool TimerFrameworkEngine::addTrg(std::shared_ptr<Trigger> tr)
{
    if (trgs_.find(tr) == trgs_.end())
    {
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
