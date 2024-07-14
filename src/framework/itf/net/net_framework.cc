#include "net_framework_engine.h"
#include "net_framework.h"

std::shared_ptr<NetframeworkEngine> Netframework::engine_;

void Netframework::init(std::shared_ptr<NetframeworkEngine> e)
{
    engine_ = e;
}

std::shared_ptr<NetframeworkEngine> Netframework::getEngine()
{
    return engine_;
}