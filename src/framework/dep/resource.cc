#include "resource.h"

using namespace std;

const std::string &Resource::getName() const
{
    return name_;
}
const std::string &Resource::getUri() const
{
    return uri_;
}
const std::string &Resource::getPath() const { return path_; }
const std::string &Resource::getHash() const
{
    return hash_;
}
const std::uint64_t Resource::getSize() const
{
    return size_;
}

void Resource::setHash(std::string h) {
    hash_ = h;
}

bool Resource::operator==(const Resource &r) const
{
    return name_ == r.name_ &&
           uri_ == r.uri_ &&
           path_ == r.path_ &&
           hash_ == r.hash_ &&
           size_ == r.size_;
}

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

const std::string &ResourceInfo::getHash() const
{
    return hash_;
}

const uint64_t ResourceInfo::size() const
{
    return range_.size();
}
