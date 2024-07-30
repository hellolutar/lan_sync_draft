#ifndef __NETWORK_CONTEXT_H_
#define __NETWORK_CONTEXT_H_

#include "network_adapter.h"

class NetworkContext
{
protected:
    std::shared_ptr<NetworkAdapter> na_;
    NetAddr peer_;

public:
    NetworkContext(){};
    NetworkContext(std::shared_ptr<NetworkAdapter> na, NetAddr peer) : na_(na), peer_(peer) {}
    virtual ~NetworkContext() {
        na_ = nullptr;
    }

    virtual void write(std::shared_ptr<uint8_t[]> data, uint64_t size);
    virtual void write(std::shared_ptr<uint8_t[]> data, uint64_t size) const;

    const NetAddr& getPeer() const;

    bool operator==(const NetworkContext& other) const;

    bool operator<(const NetworkContext& other) const;
};

#endif