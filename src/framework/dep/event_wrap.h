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
    ~EventBaseWrap();

    event_base *getBase();

    void dispatch();
    void shutdown();
};

class BuffereventWrap : public EventAbs
{
private:
    bufferevent *buf_;

public:
    BuffereventWrap(bufferevent *b)
        : buf_(b) {}
    ~BuffereventWrap();

    bufferevent *getBuf() const;
};

class EventWrap : public EventAbs
{
private:
    event *ev_;

public:
    EventWrap(event *ev) : ev_(ev) {}
    ~EventWrap();
};

class ConnectionBaseEvent : public Connection
{
private:
    std::shared_ptr<EventAbs> ev_;

public:
    ConnectionBaseEvent(std::shared_ptr<EventAbs> ev, const NetAddr &a) : ev_(ev), Connection(a) {}
    ~ConnectionBaseEvent();

    void setEvent(std::shared_ptr<EventAbs> ev);
};

#endif
