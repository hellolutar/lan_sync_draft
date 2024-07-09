#ifndef __LOGIC_H_
#define __LOGIC_H_

#include <cstdint>
#include "framework/net/net_addr.h"

class Logic
{
public:
    Logic(/* args */) {}
    virtual ~Logic() {}

    virtual const uint64_t isExtraAllDataNow(uint8_t *data, uint64_t data_len) const = 0;
    virtual void recv(const NetAddr &peer, uint8_t *data, uint64_t size) = 0;
};

#endif