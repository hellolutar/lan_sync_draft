#ifndef __END_POINT_H_
#define __END_POINT_H_

#include "logic_core.h"

class Endpoint
{
private:
    LogicCore core_;
    NetworkAdapter ad_;

public:
    Endpoint() : core_(ad_)
    {
        auto f = bind(&LogicCore::recv, &core_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        ad_.setLogicRecvFunc(f);
    };
    
    ~Endpoint() {}

    void run();
};

#endif