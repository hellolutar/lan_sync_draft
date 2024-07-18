#ifndef __LOGIC_H_
#define __LOGIC_H_

#include <memory>

#include "framework/dep/net_addr.h"

class Logic
{
public:
    Logic(/* args */) {}
    virtual ~Logic() {}

    virtual const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t data_len) const = 0;
    virtual void recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size) = 0;
};

#endif