#include "timer_framework_engine_base_event.h"

#include <event2/event.h>

#include "log/log.h"

using namespace std;

bool TriggerWrap::add(std::shared_ptr<Trigger> tr)
{
    for (auto it = trgs_.begin(); it != trgs_.end(); it++)
    {
        auto t = *it;
        if (t->getId() == tr->getId())
            return false;
    }

    trgs_.insert(tr);
    return true;
}

bool TriggerWrap::remove(std::shared_ptr<Trigger> tr)
{
    for (auto it = trgs_.begin(); it != trgs_.end(); it++)
    {
        auto t = *it;
        if (t->getId() == tr->getId())
        {
            trgs_.erase(it);
            return true;
        }
    }

    return false;
}

void TriggerWrap::tick(uint64_t t)
{
    since_last_second_ += t;
    if (since_last_second_ >= interval_)
    {
        since_last_second_ = 0;

        for (auto it = trgs_.begin(); it != trgs_.end(); it++)
        {
            auto t = *it;
            t->trigger();
        }
    }
}

void TriggerWrap::setInterval(uint64_t i)
{
    interval_ = i;
}

void timeout_cb(evutil_socket_t, short, void *arg)
{
    TimerFrameworkEngineBaseEvent *eg = (TimerFrameworkEngineBaseEvent *)arg;
    eg->tick();
}

TimerFrameworkEngineBaseEvent::~TimerFrameworkEngineBaseEvent()
{
    DEBUG("TimerFrameworkEngineBaseEvent::~TimerFrameworkEngineBaseEvent()");
    trg_map_.clear();
    once_trg_.clear();
}

bool TimerFrameworkEngineBaseEvent::addTrg(std::shared_ptr<Trigger> tr)
{
    if (tr->getPersist())
    {
        uint64_t k = tr->getPeriod().tv_sec;

        auto it = trg_map_.find(k);
        if (it == trg_map_.end())
        {
            trg_map_[k] = TriggerWrap(k);
            auto &v = trg_map_[k];
            v.add(tr);
        }
        else
        {
            auto &tw = trg_map_[k];
            return tw.add(tr);
        }
    }
    else
    {
        once_trg_.push_back(tr);
        return true;
    }
}

bool TimerFrameworkEngineBaseEvent::delTrg(std::shared_ptr<Trigger> tr)
{
    if (tr->getPersist())
    {
        uint64_t k = tr->getPeriod().tv_sec;
        auto it = trg_map_.find(k);
        if (it == trg_map_.end())
            return false;

        auto &v = trg_map_[k];
        return v.remove(tr);
    }
    else
    {
        for (auto it = once_trg_.begin(); it != once_trg_.end(); it++)
        {
            auto t = *it;
            if (t->getId() == tr->getId())
            {
                once_trg_.erase(it);
                return true;
            }
        }
    }

    return false;
}

void TimerFrameworkEngineBaseEvent::run()
{
    if (is_running_)
        return;

    is_running_ = true;

    int flag = EV_TIMEOUT | EV_PERSIST;

    struct event *timeout = event_new(base_->getBase(), -1, flag, timeout_cb, this);

    tv_.tv_usec = 0;
    tv_.tv_sec = 1;
    event_add(timeout, &tv_);

    base_->dispatch();
}

void TimerFrameworkEngineBaseEvent::shutdown()
{
    base_->shutdown();
}

void TimerFrameworkEngineBaseEvent::tick()
{
    if (once_trg_.size() > 0)
    {
        for (auto it = once_trg_.end() - 1; it >= once_trg_.begin(); it--)
        {
            auto t = *it;
            t->trigger();
            once_trg_.erase(it);
        }
    }

    for (auto &&kv : trg_map_)
    {
        auto &v = kv.second;
        v.tick(tv_.tv_sec);
    }
}
