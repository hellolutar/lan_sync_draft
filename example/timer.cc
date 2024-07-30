#include <iostream>
#include <chrono>
#include <iomanip>

#include "framework/itf/timer/timer_trigger.h"
#include "framework/itf/timer/timer_trigger_framework.h"
#include "framework/impl/timer/timer_framework_engine_base_event.h"

using namespace std;

class DemoTrigger : public Trigger
{
private:
    uint8_t c_ = 0;

public:
    DemoTrigger(timeval period, bool persist) : Trigger(period, persist){};
    ~DemoTrigger() {}

    void trigger()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(now);
        auto ptm = std::localtime(&tt);
        cout << std::put_time(ptm, "%Y-%m-%d %H:%M:%S") << " : " << to_string(c_) << endl;

        c_++;
        if (c_ > 5)
        {
            auto eg = TimerFramework::getEngine();
            eg->shutdown();
        }
    }
};

int main(int argc, char const *argv[])
{
    auto _base = make_shared<EventBaseWrap>(event_base_new());
    auto _timer = make_shared<TimerFrameworkEngineBaseEvent>(_base);

    TimerFramework::init(_timer);

    auto eg = TimerFramework::getEngine();

    timeval t;
    t.tv_sec = 2;
    eg->addTrg(make_shared<DemoTrigger>(t, true));
    eg->run();

    return 0;
}
