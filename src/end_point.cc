#include "end_point.h"

using namespace std;

void Endpoint::run()
{
    na_->start(core_);
    // auto eg = TimerFramework::getEngine();

    // auto road_id = na_->setUpSessionWithPeer(NetAddr("192.168.0.1:38080"));
    // NetworkContext ctx(na_,road_id);
    // auto trg = make_shared<UdpCliTrigger>(ctx, Trigger::second(2), true);
    // eg->addTrg(trg);
    // eg->run();
}
