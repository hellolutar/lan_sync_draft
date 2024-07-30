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

void PersistFrameworkEgine::addReadMonitor(std::shared_ptr<PersistMonitor> monitor)
{
    mons_.push_back(monitor);
}
