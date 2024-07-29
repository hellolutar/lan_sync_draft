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

void EventBaseWrap::release()
{
    if (!isRelease())
    {
        release_ = true;
    }
    throw "todo EventBaseWrap::release()";
}

BuffereventWrap::~BuffereventWrap()
{
    release();
}

bufferevent *BuffereventWrap::getBuf() const
{
    return buf_;
}

void BuffereventWrap::release()
{
    if (!release_ && buf_ != nullptr)
    {
        bufferevent_free(buf_); // todo 没有找到移除bufferevent的方法，请确定如何移除bufferevent
        buf_ = nullptr;
        DEBUG("BuffereventWrap::release()");
        release_ = true;
    }
}

EventWrap::~EventWrap()
{
    release();
}

void EventWrap::release()
{
    if (!release_)
    {
        release_ = true;
        event_del(ev_);
        DEBUG("EventWrap::release(): delete event");

        event_free(ev_);
        ev_ = nullptr;
        DEBUG("EventWrap::release(): free event");
    }
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

std::shared_ptr<EventAbs> ConnectionBaseEvent::getEventAbs()
{
    return ev_;
}

bool EventAbs::isRelease() const
{
    return release_;
}