#ifndef __COORDINATOR_H_
#define __COORDINATOR_H_

#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "task/task.h"
#include "network_adapter.h"
#include "framework/dep/resource.h"

/**
 * 负责协调任务
 * - 由TaskManager2具体管理任务
 */
class TaskCoordinator
{
private:
    std::shared_ptr<TaskManager> tm_;

    std::map<std::string, ResourceInfo> res_;

    std::shared_ptr<NetworkAdapter> na_;

    std::shared_ptr<ResourceManager> rm_;

    void analysis_resource(const ResourceInfo &info, const NetworkContext &ctx);

    void assignTask(ResourceInfo &info);

public:
    TaskCoordinator() {};
    TaskCoordinator(std::shared_ptr<TaskManager> tm) : tm_(tm) {};
    ~TaskCoordinator();

    void tick(std::uint64_t tick);

    std::shared_ptr<TaskManager> taskManager();

    void add_resource(const Resource &, const NetworkContext &roadId);

    void reAssignTask(const std::string uri, const Block blk, const NetworkContext &roadId);

    void setNetworkAdapter(std::shared_ptr<NetworkAdapter> na);

    void setResourceManager(std::shared_ptr<ResourceManager> rm);

    std::optional<const ResourceInfo> queryResource(const std::string &) const;
};

#endif