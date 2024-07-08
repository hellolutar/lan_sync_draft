#ifndef __NET_FRAMEWORK_ENGINE_H_
#define __NET_FRAMEWORK_ENGINE_H_

#include "net/net_framework.h"

class NetframeworkEngine : public NetFramework
{
private:
    static std::shared_ptr<NetFramework> engine_;

public:
    NetframeworkEngine(){};
    ~NetframeworkEngine(){};

    static void init(std::shared_ptr<NetFramework> e);
    static std::shared_ptr<NetFramework> getEngine();
};

#endif