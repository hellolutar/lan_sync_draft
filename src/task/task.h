#ifndef __TASK_H_
#define __TASK_H_

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

#include "constants/constants.h"
#include "framework/net/transport_proto.h"
#include "task/range.h"
#include "proto/lan_share_protocol.h"
#include "buf/buf_base_on_event.h"
#include "network_adapter.h"
#include "network_context.h"

class Block
{
public:
    uint64_t start;
    uint64_t end;
    Block(){};
    Block(uint64_t start, uint64_t end) : start(start), end(end){};
    ~Block(){};

    const bool operator==(const Block &other);

    const bool operator==(const Block &other) const;

    static const uint64_t pos(uint64_t offset);
    static const uint64_t bitPos(uint64_t pos);
};

enum class TaskStatusEnum
{
    Pendding,
    Syning,
    Stop,
    Success,
    Failed
};

class Task
{
private:
    std::string uri_;
    Block block_;
    NetworkContext ctx_;
    uint8_t retry_ = 0;
    uint64_t passed_time_ = 0;
    TaskStatusEnum status_ = TaskStatusEnum::Pendding;

    const uint8_t *prepareHeader() const;

public:
    Task(std::string uri, Block blk, const NetworkContext &ctx) : uri_(uri), block_(blk), ctx_(ctx){};
    // Task(const Task &&t) : uri_(t.uri_), block_(t.block_), ctx_(t.ctx_){
    // };
    Task &operator=(const Task t);

    ~Task();

    void req();
    void tick(uint64_t t);
    bool isFailed();
    void stop();
    TaskStatusEnum getStatus() const;
    void setStatus(TaskStatusEnum st);

    const std::string getUri() const;
    const Block getBlock() const;

    const NetworkContext &getNetCtx() const;
};

class TaskManager
{
protected:
    std::map<std::string, std::vector<Task>> tasks_;
    std::uint8_t download_num_ = 0;
    uint64_t replaceStatusByStatus(std::string, TaskStatusEnum oldst, TaskStatusEnum newst);

public:
    TaskManager();
    virtual ~TaskManager();

    void addTask(const Task &t);
    void cancelTask(std::string uri);

    void tick(std::function<void(const std::string uri, const Block blk, const NetworkContext &oldCtx)> reAssignTaskFunc);
    void tick(uint64_t t, std::function<void(const std::string uri, const Block blk, const NetworkContext &oldCtx)> reAssignTaskFunc);

    uint64_t stopPendingTask(std::string);
    uint64_t pendingStopTask(std::string);

    void success(std::string uri, Block block);

    const bool isSuccess(std::string uri) const;

    const uint64_t downloadNum() const;
};

#endif