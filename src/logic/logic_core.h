#ifndef __LOGIC_CORE_H_
#define __LOGIC_CORE_H_

#include "udp_cli_trigger.h"
#include "network_adapter.h"
#include "resource_manager.h"
#include "task/task_coordinator.h"
#include "conf/properties_parse.h"

class LogicCore : public LogicWrite
{
private:
    std::shared_ptr<NetworkAdapter> adapter_;
    std::shared_ptr<ResourceManager> rm_;
    std::shared_ptr<TaskCoordinator> coor_;
    std::shared_ptr<UdpCliTrigger> hello_trg_;
    std::shared_ptr<PropertiesParse> conf_;

    void helloAck(NetAbilityContext &ctx, const LanSyncPkt &pkt);
    void reqIdx(NetAbilityContext &ctx, const LanSyncPkt &pkt);
    void reqRs(NetAbilityContext &ctx, const LanSyncPkt &pkt);
    void recvIdx(NetAbilityContext &ctx, const LanSyncPkt &pkt);
    void recvRs(NetAbilityContext &ctx, const LanSyncPkt &pkt);
    void shutdown(NetAbilityContext &ctx, const LanSyncPkt &pkt);

    void readDataThenReplyRs(const std::string &uri, const Block &b, NetAbilityContext &ctx);

public:
    LogicCore() {};
    LogicCore(std::shared_ptr<NetworkAdapter> adapter,
              std::shared_ptr<ResourceManager> rm)
        : adapter_(adapter), rm_(rm) {}
    ~LogicCore() {}

    const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data,
                                     uint64_t data_len) const override;
    void recv(NetAbilityContext &ctx, std::shared_ptr<uint8_t[]> data, uint64_t size) override;

    void setNetworkAdapter(std::shared_ptr<NetworkAdapter> ad);
    void setResourceManager(std::shared_ptr<ResourceManager> rm);
    void setTaskCoordinator(std::shared_ptr<TaskCoordinator> coor);
    void setUdpCliTrigger(std::shared_ptr<UdpCliTrigger> trg);
    void setPropertiesParse(std::shared_ptr<PropertiesParse> conf);
};

#endif