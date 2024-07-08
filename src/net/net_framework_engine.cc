#include "net_framework_engine.h"

std::shared_ptr<NetFramework> NetframeworkEngine::engine_;

void NetframeworkEngine::init(std::shared_ptr<NetFramework> e)
{
    engine_ = e;
}

std::shared_ptr<NetFramework> NetframeworkEngine::getEngine()
{
    return engine_;
}