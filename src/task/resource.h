#ifndef __RESOURCE_H_
#define __RESOURCE_H_

#include <vector>
#include <string>
#include <memory>

#include "task/range.h"
#include "network_context.h"
#include "framework/itf/net/transport_proto.h"

class ResourceInfo
{
private:
    std::string uri_;
    Range range_;
    std::vector<NetworkContext> ctxs_;

public:
    ResourceInfo(std::string uri, Range range) : uri_(uri), range_(range){};
    ~ResourceInfo();

    const Range &getRange() const;

    const std::string getUri() const;

    void addNetCtx(const NetworkContext& ctx);

    const std::vector<NetworkContext> &getNetCtxs() const;

    const uint64_t size() const;
};

#endif