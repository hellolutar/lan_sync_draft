#ifndef __LOGIC_CORE_H_
#define __LOGIC_CORE_H_

#include "network_adapter.h"
#include "resource_manager.h"
#include "task/task_coordinator.h"

class LogicCore : public Logic
{
private:
    std::shared_ptr<NetworkAdapter> adapter_;
    std::shared_ptr<ResourceManager> rm_;
    std::shared_ptr<TaskCoordinator> coor_;

    void helloAck(const NetAddr &peer, const LanSyncPkt &pkt);
    void reqIdx(const NetAddr &peer, const LanSyncPkt &pkt);
    void reqRs(const NetAddr &peer, const LanSyncPkt &pkt);
    void recvIdx(const NetAddr &peer, const LanSyncPkt &pkt);
    void recvRs(const NetAddr &peer, const LanSyncPkt &pkt);
    void shutdown(const NetAddr &peer, const LanSyncPkt &pkt);

    void readDataThenReplyRs(const std::string &uri, const Block &b, const NetAddr &peer);

public:
    LogicCore(){};
    LogicCore(std::shared_ptr<NetworkAdapter> adapter,
              std::shared_ptr<ResourceManager> rm)
        : adapter_(adapter), rm_(rm) {}
    ~LogicCore() {}

    const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data,
                                     uint64_t data_len) const override;
    void recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size) override;

    void setNetworkAdapter(std::shared_ptr<NetworkAdapter> ad);
    void setResourceManager(std::shared_ptr<ResourceManager> rm);
    void setTaskCoordinator(std::shared_ptr<TaskCoordinator> coor);
};

#endif