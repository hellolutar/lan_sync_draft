#ifndef __EVENT_WRAP_H_
#define __EVENT_WRAP_H_

#include <event2/bufferevent.h>

class EventBaseWrap
{
private:
    event_base *base_;

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
    const bufferevent *get() const
    {
        return buf_;
    }
};

#endif
