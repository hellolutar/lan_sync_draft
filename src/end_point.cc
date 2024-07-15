#include "end_point.h"

using namespace std;

void Endpoint::registerTrg()
{
    NetworkContext ctx(na_, NetAddr("0.0.0.1:8080"));
    hello_trg_ = std::make_shared<UdpCliTrigger>(ctx, Trigger::second(2), true);
    auto timer = TimerFramework::getEngine();
    timer->addTrg(hello_trg_); // todo cancel trigger when the connect was established.

    auto coor = make_shared<TaskCoordinator>(tm_);
    coor_trg_ = make_shared<TaskCoordinatorTrigger>(Trigger::second(2), true, coor);
    timer->addTrg(hello_trg_);
    core_->setTaskCoordinator(coor);
}

void Endpoint::run()
{
    registerTrg();

    na_->start(core_);

    TimerFramework::getEngine()->run();
}

std::shared_ptr<LogicCore> Endpoint::getLogicCore() { return core_; }

void Endpoint::setTaskManager(std::shared_ptr<TaskManager> tm) {
    tm_ = tm;
}
