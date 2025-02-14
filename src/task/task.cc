#include "task/task.h"
#include "task.h"
#include "log/log.h"

using namespace std;

Task &Task::operator=(const Task t)
{
    uri_ = t.uri_;
    block_ = t.block_;
    ctx_ = t.ctx_;
    return *this;
}

Task::~Task()
{
}

void Task::req()
{
    status_ = TaskStatusEnum::Syning;

    LanSyncPkt pkt(lan_sync_version::VER_0_1, LAN_SYNC_TYPE_GET_RESOURCE);

    string range_hdr = Range(block_.start, block_.end).to_string();
    pkt.addXheader(XHEADER_URI, uri_);
    pkt.addXheader(XHEADER_RANGE, range_hdr);

    BufBaseonEvent buf;
    pkt.writeTo(buf);

    try
    {
        DEBUG_F("Task::req()", " {} {} {} ", uri_, block_.str(), ctx_.getPeer().str());
        ctx_.write(buf.data(), buf.size());
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        retry_++;
        status_ = TaskStatusEnum::Failed;
    }
}

void Task::tick(uint64_t t)
{
    passed_time_ += t;
    if (passed_time_ >= 1000 * 60)
    {
        retry_++;
        passed_time_ = 0;
        // todo(lutar, 20240628) may be request again
    }
    if (retry_ >= MAX_RETRY)
    {
        DEBUG_F("Task::tick()", " tag Failed: {} {} {} ", uri_, block_.str(), ctx_.getPeer().str());
        status_ = TaskStatusEnum::Failed;
    }
}

bool Task::isFailed()
{
    if (retry_ >= 5)
        return true;
    return false;
}

void Task::stop()
{
    status_ = TaskStatusEnum::Stop;
}

TaskStatusEnum Task::getStatus() const
{
    return status_;
}

void Task::setStatus(TaskStatusEnum st)
{
    status_ = st;
}

const std::string Task::getUri() const
{
    return uri_;
}

const Block Task::getBlock() const
{
    return block_;
}

const uint64_t Task::getTotalSize() const { return total_size_; }

const NetworkContext &Task::getNetCtx() const { return ctx_; }

TaskManager::TaskManager(/* args */)
{
}

TaskManager::~TaskManager()
{
}

void TaskManager::addTask(const Task &t)
{
    auto uri = t.getUri();
    auto uri_iter = tasks_.find(uri);

    if (uri_iter == tasks_.end())
    {
        tasks_[uri] = std::vector<Task>();
        tasks_[uri].push_back(t);
    }
    else
    {
        // todo 要确认是否重复
        uint64_t pos = Block::pos(t.getBlock().start);
        if (pos >= tasks_[uri].size())
            tasks_[uri].push_back(t);
        else
        {
            if (tasks_[uri][pos].getStatus() == TaskStatusEnum::Success)
                return;
            tasks_[uri][pos] = t;
        }
    }
    DEBUG_F("TaskManager::addTask()", " {} | {} | {}", uri, t.getBlock().str(), t.getNetCtx().getPeer().str());
}

void TaskManager::cancelTask(std::string uri)
{
    auto iter = tasks_.find(uri);
    if (iter != tasks_.end())
    {
        // todo( lutar,20240701) remove local tmp file
        tasks_.erase(iter);
        DEBUG_F("TaskManager::cancelTask()", " {} ", uri);
    }
}

void TaskManager::tick(std::function<void(const std::string uri, const Block blk, const NetworkContext &oldCtx)> reAssignTaskFunc)
{
    tick(0, reAssignTaskFunc);
}

void TaskManager::tick(uint64_t t, std::function<void(const std::string uri, const Block blk, const NetworkContext &oldCtx)> reAssignTaskFunc)
{
    for (auto uri_iter = tasks_.begin(); uri_iter != tasks_.end(); uri_iter++)
    {
        std::vector<Task> &ts = uri_iter->second;

        for (auto t_iter = ts.begin(); t_iter != ts.end(); t_iter++)
        {
            Task &tsk = *t_iter;
            if (tsk.getStatus() == TaskStatusEnum::Success)
                continue;

            if (tsk.getStatus() == TaskStatusEnum::Pendding)
            {
                if (download_num_ >= DOWNLOAD_LIMIT)
                    continue;
                tsk.req();
                download_num_++;
            }
            else if (tsk.getStatus() == TaskStatusEnum::Syning)
            {
                tsk.tick(t);
            }
            else if (tsk.getStatus() == TaskStatusEnum::Failed)
            {
                reAssignTaskFunc(tsk.getUri(), tsk.getBlock(), tsk.getNetCtx());
            }
        }
    }
}

uint64_t TaskManager::stopPendingTask(const std::string &uri)
{
    return replaceStatusByStatus(uri, TaskStatusEnum::Pendding, TaskStatusEnum::Stop);
}

uint64_t TaskManager::pendingStopTask(const std::string &uri)
{
    return replaceStatusByStatus(uri, TaskStatusEnum::Stop, TaskStatusEnum::Pendding);
}

uint64_t TaskManager::replaceStatusByStatus(std::string uri, TaskStatusEnum oldst, TaskStatusEnum newst)
{
    auto uri_iter = tasks_.find(uri);
    if (uri_iter == tasks_.end())
        return 0;

    std::vector<Task> &ts = uri_iter->second;
    int i = 0;

    for (auto t_iter = ts.begin(); t_iter != ts.end(); t_iter++)
    {
        Task &tsk = *t_iter;
        if (tsk.getStatus() == oldst)
        {
            tsk.setStatus(newst);
            i++;
        }
    }
    return i;
}

void TaskManager::success(const std::string &uri, Block block)
{
    auto uri_iter = tasks_.find(uri);
    if (uri_iter == tasks_.end())
        return;

    std::vector<Task> &ts = uri_iter->second;
    for (uint64_t i = 0; i < ts.size(); i++)
    {
        if (ts[i].getBlock() == block)
        {
            ts[i].setStatus(TaskStatusEnum::Success);
            DEBUG_F("TaskManager::success()", " {} {}", uri, block.str());
            download_num_--;
        }
    }
}

void TaskManager::fail(const std::string &uri)
{
    auto uri_iter = tasks_.find(uri);
    if (uri_iter == tasks_.end())
        return;

    std::vector<Task> &ts = uri_iter->second;
    for (uint64_t i = 0; i < ts.size(); i++)
    {
        ts[i].setStatus(TaskStatusEnum::Failed);
        DEBUG_F("TaskManager::fail()", " {} ", uri);
        download_num_--;
    }
}

const bool TaskManager::isSuccess(std::string uri) const
{
    auto uri_iter = tasks_.find(uri);
    if (uri_iter == tasks_.end())
        return false;

    const std::vector<Task> &ts = uri_iter->second;
    for (uint64_t i = 0; i < ts.size(); i++)
    {
        if (ts[i].getStatus() != TaskStatusEnum::Success)
            return false;
    }
    return true;
}

const uint64_t TaskManager::downloadNum() const
{
    return download_num_;
}
