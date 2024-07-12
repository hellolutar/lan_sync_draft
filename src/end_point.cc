#include "end_point.h"

using namespace std;

void Endpoint::registerTrg()
{
    NetworkContext ctx(na_, NetAddr("0.0.0.1:8080"));
    trg_ = std::make_shared<UdpCliTrigger>(ctx, Trigger::second(2), true);
    auto timer = TimerFramework::getEngine();
    timer->addTrg(trg_);
}

void Endpoint::run()
{
    registerTrg();

    na_->start(core_);

    TimerFramework::getEngine()->run();
}

std::shared_ptr<LogicCore> Endpoint::getLogicCore()
{
    return core_;
}
