#ifndef __NET_FRAMEWORK_ENGINE_H_
#define __NET_FRAMEWORK_ENGINE_H_

#include "framework/net/net_framework_engine.h"

class Netframework
{
protected:
    static std::shared_ptr<NetframeworkEngine> engine_;

public:
    Netframework(){};
    ~Netframework(){};

    static void init(std::shared_ptr<NetframeworkEngine> e);
    static std::shared_ptr<NetframeworkEngine> getEngine();
};

#endif