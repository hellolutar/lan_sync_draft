#ifndef __LOGIC_CORE_H_
#define __LOGIC_CORE_H_

#include "network_adapter.h"

class LogicCore
{
private:
    std::shared_ptr<NetworkAdapter> adapter_;

public:
    LogicCore(std::shared_ptr<NetworkAdapter> adapter) : adapter_(adapter) {}
    ~LogicCore() {}

    void recv(uint32_t road_id, uint8_t *data, uint64_t size);
};

#endif