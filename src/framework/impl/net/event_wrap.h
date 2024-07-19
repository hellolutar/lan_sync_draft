#ifndef __EVENT_WRAP_H_
#define __EVENT_WRAP_H_

#include <iostream>
#include <memory>

#include <event2/event.h>
#include <event2/bufferevent.h>

#include "framework/dep/connection.h"

class EventAbs
{
public:
    EventAbs(/* args */) {}
    virtual ~EventAbs() {}
};

class EventBaseWrap : public EventAbs
{
private:
    event_base *base_;
    bool dispatched_ = false;

public:
    EventBaseWrap(event_base *b) : base_(b) {}
    ~EventBaseWrap()
    {
        // todo free event_base
    }

    event_base *getBase()
    {
        return base_;
    }

    void dispatch()
    {
        if (dispatched_)
            return;

        dispatched_ = true;
        event_base_dispatch(base_);
    }

    void shutdown()
    {
        event_base_loopbreak(base_);
    }
};

class BuffereventWrap : public EventAbs
{
private:
    bufferevent *buf_;

public:
    BuffereventWrap(bufferevent *b)
        : buf_(b)
    {
    }
    ~BuffereventWrap()
    {
        if (buf_ != nullptr)
        {
            bufferevent_free(buf_); // todo 没有找到移除bufferevent的方法，请确定如何移除bufferevent
            buf_ = nullptr;
        }
    }
    bufferevent *getBuf() const
    {
        return buf_;
    }
};

class EventWrap : public EventAbs
{
private:
    event *ev_;

public:
    EventWrap(event *ev) : ev_(ev) {}
    ~EventWrap()
    {
        event_del(ev_);

        event_free(ev_);
        ev_ = nullptr;
    }
};

class ConnectionBaseEvent : public Connection
{
private:
    std::shared_ptr<EventAbs> ev_;

public:
    ConnectionBaseEvent(std::shared_ptr<EventAbs> ev, const NetAddr &a) : ev_(ev), Connection(a) {}
    ~ConnectionBaseEvent()
    {
        auto c = ev_.use_count();
        std::cout << " ~ConnectionBaseEvent" << std::endl;
        ev_ = nullptr;
    }

    void setEvent(std::shared_ptr<EventAbs> ev)
    {
        ev_ = ev;
    }
};

#endif
