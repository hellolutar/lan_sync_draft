#include "end_point.h"

#include "conf/properties_parse.h"

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

    auto cb = std::bind(&NetworkAdapter::rmSessionCallBack, na_.get(), std::placeholders::_1);
    auto ng = Netframework::getEngine();
    ng->setTcpDisConnCB(cb);

    if (core_ == nullptr)
        core_ = std::make_shared<LogicCore>();

    if (tm_ == nullptr)
        tm_ = std::make_shared<TaskManager>();

    auto eg = Netframework::getEngine();

    if (hello_trg_ == nullptr)
    {
        auto discover_ip = conf_->query(PropertiesParse::DISCOVER_IPS);
        auto discover_port = conf_->query(PropertiesParse::PROTO_DISCOVER_SERVER_UDP_PORT);
        auto peer = discover_ip + ":" + discover_port;
        NetworkContext ctx(na_, NetAddr(peer, TransportType::UDP));
        hello_trg_ = std::make_shared<UdpCliTrigger>(ctx, Trigger::second(2), true);
    }

    if (coor_trg_ == nullptr)
        coor_trg_ = make_shared<TaskCoordinatorTrigger>(Trigger::second(2), true, make_shared<TaskCoordinator>(tm_));

    core_->setNetworkAdapter(na_);
    core_->setTaskCoordinator(coor_trg_->getTaskCoordinator());
    core_->setResourceManager(rm_);
    core_->setUdpCliTrigger(hello_trg_);
    core_->setPropertiesParse(conf_);
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

void Endpoint::setNetworkAdapter(std::shared_ptr<NetworkAdapter> na)
{
    na_ = na;
}

void Endpoint::setResourceManager(std::shared_ptr<ResourceManager> rm)
{
    rm_ = rm;
}

void Endpoint::setPropertiesParse(std::shared_ptr<PropertiesParse> conf)
{
    conf_ = conf;
}

std::shared_ptr<UdpCliTrigger> Endpoint::getUdpCliTrigger()
{
    return hello_trg_;
}
