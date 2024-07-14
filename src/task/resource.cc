#include "task/resource.h"

ResourceInfo::~ResourceInfo()
{
}

const Range &ResourceInfo::getRange() const
{
    return range_;
}

const std::string ResourceInfo::getUri() const
{
    return uri_;
}

void ResourceInfo::addNetCtx(const NetworkContext& ctx)
{
    ctxs_.push_back(ctx);
}

const std::vector<NetworkContext> &ResourceInfo::getNetCtxs() const
{
    return ctxs_;
}

const uint64_t ResourceInfo::size() const
{
    return range_.size();
}
