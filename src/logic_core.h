#ifndef __LOGIC_CORE_H_
#define __LOGIC_CORE_H_

#include "network_adapter.h"

class LogicCore : public Logic
{
private:
    std::shared_ptr<NetworkAdapter> adapter_;

public:
    LogicCore(){};
    LogicCore(std::shared_ptr<NetworkAdapter> adapter) : adapter_(adapter) {}
    ~LogicCore() {}

    const uint64_t isExtraAllDataNow(uint8_t *data, uint64_t data_len) const override;
    void recv(const NetAddr &peer, uint8_t *data, uint64_t size) override;

    void setNetworkAdapter(std::shared_ptr<NetworkAdapter> ad);
};

#endif