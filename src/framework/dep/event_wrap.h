#ifndef __EVENT_WRAP_H_
#define __EVENT_WRAP_H_

#include <iostream>
#include <memory>

#include <event2/event.h>
#include <event2/bufferevent.h>

#include "framework/dep/connection.h"

class EventAbs
{
protected:
    bool release_ = false;

public:
    EventAbs(/* args */) {}
    virtual ~EventAbs() {}
    virtual bool isRelease() const;
    virtual void release() = 0;
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

    void release() override;
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
    void release() override;
};

class EventWrap : public EventAbs
{
private:
    event *ev_;

public:
    EventWrap(event *ev) : ev_(ev) {}
    ~EventWrap();

    void release() override;
};

class ConnectionBaseEvent : public Connection
{
protected:
    std::shared_ptr<EventAbs> ev_;

public:
    ConnectionBaseEvent(std::shared_ptr<EventAbs> ev, const NetAddr &a) : ev_(ev), Connection(a) {}
    ~ConnectionBaseEvent();

    virtual void setEvent(std::shared_ptr<EventAbs> ev);
    virtual std::shared_ptr<EventAbs> getEventAbs();
};

#endif
