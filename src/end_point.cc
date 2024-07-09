#include "end_point.h"

using namespace std;

void Endpoint::run()
{
    na_->start();
    auto eg = TimerFramework::getEngine();

    na_->connPeer(NetAddr("192.168.0.1:38080"));
    NetworkContext ctx;
    auto trg = make_shared<UdpCliTrigger>(ctx, Trigger::second(2), true);
    eg->addTrg(trg);
    eg->run();
}
