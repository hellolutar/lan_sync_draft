#ifndef __FRAMEWORK_IMPL_TIMER_TIMER_FRAMEWORK_ENGINE_BASE_EVENT_H_
#define __FRAMEWORK_IMPL_TIMER_TIMER_FRAMEWORK_ENGINE_BASE_EVENT_H_

#include <vector>

#include "framework/itf/timer/timer_trigger_framework.h"
#include "framework/dep/event_wrap.h"

class TriggerWrap
{
private:
    uint64_t interval_ = 0;
    uint64_t since_last_second_ = 0;
    std::set<std::shared_ptr<Trigger>> trgs_;

public:
    TriggerWrap(){};
    TriggerWrap(uint64_t interval) : interval_(interval) {}
    ~TriggerWrap() {}

    bool add(std::shared_ptr<Trigger> trg);
    bool remove(std::shared_ptr<Trigger> trg);
    void tick(uint64_t t);
    void setInterval(uint64_t);
};

class TimerFrameworkEngineBaseEvent : public TimerFrameworkEngine, public std::enable_shared_from_this<TimerFrameworkEngineBaseEvent>
{
private:
    bool is_running_ = false;
    std::shared_ptr<EventBaseWrap> base_;
    timeval tv_;

    std::map<uint64_t, TriggerWrap> trg_map_;
    std::vector<std::shared_ptr<Trigger>> once_trg_;

public:
    TimerFrameworkEngineBaseEvent(std::shared_ptr<EventBaseWrap> b)
        : base_(b) {}
    ~TimerFrameworkEngineBaseEvent();

    bool addTrg(std::shared_ptr<Trigger> tr) override;
    bool delTrg(std::shared_ptr<Trigger> tr) override;
    void run() override;
    void shutdown() override;

    void tick();
};

#endif