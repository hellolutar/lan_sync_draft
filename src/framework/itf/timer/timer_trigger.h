#ifndef __TIMER_TRIGGER_H_
#define __TIMER_TRIGGER_H_

#include <map>
#include <ctime>
#include <cstdint>

#include <sys/time.h>

class Trigger;

class Timer
{
protected:
public:
    Timer(/* args */);
    virtual bool addTrigger(Trigger *tr) = 0;
    virtual bool rmTrigger(Trigger *tr) = 0;
    virtual ~Timer();
};

class TriggerBehavior
{
public:
    virtual ~TriggerBehavior(){};
    virtual void trigger() = 0;
};

class Trigger : public TriggerBehavior
{
protected:
    struct timeval period;

    bool persist;

    std::uint16_t id_;

public:
    Trigger(struct timeval period, bool persist) : period(period), persist(persist){};
    virtual ~Trigger();

    virtual struct timeval &getPeriod();
    virtual bool &getPersist();

    virtual void setId(std::uint16_t id);
    virtual std::uint16_t getId() const;

    static timeval second(size_t s);
};

#endif
