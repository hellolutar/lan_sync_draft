#include "event_wrap.h"
#include "log/log.h"

EventBaseWrap::~EventBaseWrap()
{
    // todo free event_base
    DEBUG("EventBaseWrap::~EventBaseWrap()");
}

event_base *EventBaseWrap::getBase() { return base_; }

void EventBaseWrap::dispatch()
{
    if (dispatched_)
        return;

    dispatched_ = true;

    DEBUG("EventBaseWrap::dispatch()");

    event_base_dispatch(base_);
}

void EventBaseWrap::shutdown()
{
    event_base_loopbreak(base_);
    DEBUG("EventBaseWrap::shutdown()");
}

BuffereventWrap::~BuffereventWrap()
{
    {
        if (buf_ != nullptr)
        {
            bufferevent_free(buf_); // todo 没有找到移除bufferevent的方法，请确定如何移除bufferevent
            buf_ = nullptr;
            DEBUG("BuffereventWrap::~BuffereventWrap()");
        }
    }
}

bufferevent *BuffereventWrap::getBuf() const
{
    return buf_;
}

EventWrap::~EventWrap()
{
    event_del(ev_);
    DEBUG("EventWrap::~EventWrap(): delete event");

    event_free(ev_);
    ev_ = nullptr;
    DEBUG("EventWrap::~EventWrap(): free event");
}

ConnectionBaseEvent::~ConnectionBaseEvent()
{
    ev_ = nullptr;
    DEBUG("ConnectionBaseEvent::~ConnectionBaseEvent()");
}

void ConnectionBaseEvent::setEvent(std::shared_ptr<EventAbs> ev)
{
    ev_ = ev;
}
