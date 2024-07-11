#include "persist_framework.h"

std::shared_ptr<PersistFrameworkEgine> PersistFramework::engine_;

void PersistFramework::init(std::shared_ptr<PersistFrameworkEgine> eg)
{
    engine_ = eg;
}

std::shared_ptr<PersistFrameworkEgine> PersistFramework::getEngine()
{
    return engine_;
}
