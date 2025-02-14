#include "task/task_coordinator.h"
#include "task_coordinator.h"
#include "log/log.h"

using namespace std;

void TaskCoordinator::analysis_resource(const ResourceInfo &info, const NetworkContext &ctx)
{
    std::string uri = info.getUri();
    auto r_iter = res_.find(uri);

    if (r_iter == res_.end()) // 为新资源
    {
        res_.insert(std::pair<std::string, ResourceInfo>(info.getUri(), info));

        DEBUG_F("TaskCoordinator::analysis_resource()", " new resource : \n\t"
                "uri: {} \n\t"
                "range: {}",
                info.getUri(),
                info.getRange().to_string());
    }
    else
    {
        // 找到了， 判断是否需要更新资源
        ResourceInfo &old_r = r_iter->second;
        if (old_r.getRange() < info.getRange())
        {
            tm_->cancelTask(uri);                                                   // 取消任务， todo 删除本地文件
            res_.erase(r_iter);                                                     // 资源表中移除
            res_.insert(std::pair<std::string, ResourceInfo>(info.getUri(), info)); // 向资源表插入新资源信息

            DEBUG_F("TaskCoordinator::analysis_resource()", " Discover the LATEST resource : \n\t"
                    "uri: {} \n\t"
                    "old: {} \n\t"
                    "new: {}",
                    info.getUri(),
                    old_r.getRange().to_string(),
                    info.getRange().to_string());
        }
        else if (old_r.getRange() == info.getRange())
        {
            old_r.addNetCtx(ctx);

            DEBUG_F("TaskCoordinator::analysis_resource()", " new owner : \n\t"
                    "uri: {} \n\t"
                    "range: {}",
                    info.getUri(),
                    ctx.getPeer().str());
        }
        else
            return; // 新资源为过时资源，故跳过
    }
    r_iter = res_.find(uri);
    ResourceInfo &r = r_iter->second;
    assignTask(r); // 指派任务
}

void TaskCoordinator::assignTask(ResourceInfo &info)
{
    auto &ctxs = info.getNetCtxs();
    if (ctxs.size() == 0)
    {
        WARN_F("TaskCoordinator::assignTask()", " cannot assign task, because the contexts is empty!");
        return;
    }

    tm_->stopPendingTask(info.getUri());

    uint16_t who = 0;
    if (info.size() <= BLOCK_SIZE)
    {
        string uri = info.getUri();
        auto &ctx = const_cast<NetworkContext &>(ctxs[who]); // todo 可能存在拷贝陷进
        tm_->addTask({uri, info.size(), Block(0, info.size()), ctx, rm_});
    }
    else
    {
        uint num = info.size() / BLOCK_SIZE + 1; // 向上取整
        for (uint i = 0; i < num; i++)
        {
            if (who >= ctxs.size())
                who = 0;
            uint64_t bitPos = Block::bitPos(i);
            if (bitPos >= info.size())
                break;

            uint64_t end = min(info.size(), bitPos + BLOCK_SIZE);
            string uri = info.getUri();
            auto &ctx = const_cast<NetworkContext &>(ctxs[who++]); // 可能存在拷贝陷进
            Task tsk = {uri, info.size(), Block(bitPos, end), ctx, rm_};
            tm_->addTask(tsk);
        }
    }
    tm_->pendingStopTask(info.getUri());
    tick(0);
}

TaskCoordinator::~TaskCoordinator()
{
}

void TaskCoordinator::tick(std::uint64_t t)
{
    auto f = bind(&TaskCoordinator::reAssignTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tm_->tick(t, f);
}

std::shared_ptr<TaskManager> TaskCoordinator::taskManager()
{
    return tm_;
}

void TaskCoordinator::add_resource(const Resource &res, const NetworkContext &ctx)
{
    ResourceInfo r{res.getUri(), res.getHash(), {0, res.getSize()}};
    r.addNetCtx(ctx);
    analysis_resource(r, ctx);
}

void TaskCoordinator::reAssignTask(const std::string uri, const Block blk, const NetworkContext &ctx)
{
    auto r_iter = res_.find(uri);
    ResourceInfo &r = r_iter->second;

    NetworkContext newctx;
    auto &rs = r.getNetCtxs();
    for (size_t i = 0; i < rs.size(); i++)
    {
        if (rs[i] == ctx)
        {
            if (i == rs.size() - 1)
                newctx = rs[0];
            else
            {
                newctx = rs[i + 1];
            }
            DEBUG_F("TaskCoordinator::reAssignTask()", " {} {} | {} --> {} ", uri, blk.str(), ctx.getPeer().str(), newctx.getPeer().str());
            tm_->addTask({r.getUri(), r.size(), blk, newctx, rm_});
            break;
        }
    }
}

void TaskCoordinator::setNetworkAdapter(std::shared_ptr<NetworkAdapter> na)
{
    na_ = na;
}

void TaskCoordinator::setResourceManager(std::shared_ptr<ResourceManager> rm)
{
    rm_ = rm;
}

std::optional<const ResourceInfo> TaskCoordinator::queryResource(const std::string &uri) const
{
    auto it = res_.find(uri);
    if (it == res_.end())
        return nullopt;

    return make_optional<const ResourceInfo>(it->second);
}
