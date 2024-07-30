#ifndef __RESOURCE_H_
#define __RESOURCE_H_

#include <vector>
#include <string>
#include <memory>

#include "network_context.h"
#include "framework/dep/block_range.h"
#include "framework/itf/net/transport_proto.h"

class Resource
{
private:
    std::string name_;
    std::string uri_;
    std::string path_;
    std::string hash_;
    std::uint64_t size_;

public:
    Resource(/* args */) {}
    Resource(std::string name, std::string uri, std::string path, std::string hash, std::uint64_t size)
        : name_(name), uri_(uri), path_(path), hash_(hash), size_(size) {}
    ~Resource() {}

    const std::string &getName() const;
    const std::string &getUri() const;
    const std::string &getPath() const;
    const std::string &getHash() const;
    const std::uint64_t getSize() const;
    void setHash(std::string);

    bool operator==(const Resource &r) const;
};

class ResourceInfo
{
private:
    std::string uri_;
    std::string hash_;
    Range range_;
    std::vector<NetworkContext> ctxs_;

public:
    ResourceInfo(std::string uri, std::string hash, Range range) : uri_(uri), hash_(hash), range_(range) {};
    ~ResourceInfo();

    const Range &getRange() const;

    const std::string getUri() const;

    void addNetCtx(const NetworkContext &ctx);

    const std::vector<NetworkContext> &getNetCtxs() const;

    const std::string& getHash() const;

    const uint64_t size() const;
};

#endif