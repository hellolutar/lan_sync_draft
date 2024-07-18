#ifndef __EVENT_WRAP_H_
#define __EVENT_WRAP_H_

#include <event2/event.h>
#include <event2/bufferevent.h>

class EventBaseWrap
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
};

class BuffereventWrap
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
            bufferevent_free(buf_);
            buf_ = nullptr;
        }
    }
    bufferevent *getBuf() const
    {
        return buf_;
    }
};

#endif
