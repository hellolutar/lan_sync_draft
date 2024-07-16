#include "end_point.h"

using namespace std;

void Endpoint::registerTrg()
{
    auto timer = TimerFramework::getEngine();
    timer->addTrg(hello_trg_); // todo cancel trigger when the connect was established.
    timer->addTrg(hello_trg_);
    timer->addTrg(coor_trg_);
}

void Endpoint::init()
{
    if (na_ == nullptr)
        na_ = std::make_shared<NetworkAdapter>();

    if (core_ == nullptr)
        core_ = std::make_shared<LogicCore>();

    if (tm_ == nullptr)
        tm_ = std::make_shared<TaskManager>();

    if (hello_trg_ == nullptr)
    {
        NetworkContext ctx(na_, NetAddr("0.0.0.1:8080"));
        hello_trg_ = std::make_shared<UdpCliTrigger>(ctx, Trigger::second(2), true);
    }

    if (coor_trg_ == nullptr)
        coor_trg_ = make_shared<TaskCoordinatorTrigger>(Trigger::second(2), true, make_shared<TaskCoordinator>(tm_));

    core_->setNetworkAdapter(na_);
    core_->setTaskCoordinator(coor_trg_->getTaskCoordinator());
}

void Endpoint::run()
{
    init();
    registerTrg();

    na_->start(core_);

    TimerFramework::getEngine()->run();
}

void Endpoint::setTaskManager(std::shared_ptr<TaskManager> tm)
{
    tm_ = tm;
}
