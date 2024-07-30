#include "framework/itf/timer/timer_trigger.h"
#include "timer_trigger.h"

using namespace std;
Timer::Timer(/* args */)
{
}

Timer::~Timer(/* args */)
{
}

Trigger::~Trigger()
{
}

timeval Trigger::second(size_t s)
{
    struct timeval tv;
    tv.tv_usec = 0;
    tv.tv_sec = s;
    return tv;
}

void Trigger::setId(std::uint16_t id)
{
    id_ = id;
}

std::uint16_t Trigger::getId() const
{
    return id_;
}

struct timeval &Trigger::getPeriod()
{
    return period;
}
bool &Trigger::getPersist()
{
    return persist;
}