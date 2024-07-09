#ifndef __NETWORK_CONTEXT_H_
#define __NETWORK_CONTEXT_H_

#include "network_adapter.h"

class NetworkContext
{
protected:
    std::shared_ptr<NetworkAdapter> na_;
    uint32_t road_id_ = 0;

public:
    NetworkContext(){};
    NetworkContext(std::shared_ptr<NetworkAdapter> na, uint32_t road_id) : na_(na), road_id_(road_id) {}
    virtual ~NetworkContext() {
        na_ = nullptr;
        road_id_ = 0;
    }

    virtual void write(uint8_t* data, uint64_t size);
    virtual void write(uint8_t* data, uint64_t size) const;

    bool operator==(const NetworkContext& other) const;
};

#endif