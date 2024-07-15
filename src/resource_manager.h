#ifndef __RESOURCE_MANAGER_H_
#define __RESOURCE_MANAGER_H_

#include <vector>
#include <map>
#include <filesystem>
#include <memory>
#include <optional>

#include "exc/not_found_exc.h"
#include "framework/dep/resource.h"
#include "framework/dep/block_range.h"
#include "framework/itf/persist/persist_framework.h"

class ResourceManager
{
private:
public:
    ResourceManager(/* args */) {}
    virtual ~ResourceManager() {}

    virtual std::vector<Resource> idx() = 0;
    virtual const Resource &query(std::string uri) const = 0;
    virtual bool validRes(std::string uri, std::string hash) const = 0;
    virtual std::vector<struct Resource> need_to_sync(std::vector<struct Resource> peer_table) const = 0;
    virtual bool save(std::string uri, void *data, uint64_t offset, uint64_t data_len) = 0;

    virtual std::optional<std::shared_ptr<uint8_t[]>> readFrom(std::string uri, const Block &blk) const = 0;
};

class ResourceManagerBaseFilesystem : public ResourceManager
{
private:
    std::string rsHome;
    // mapping for uri and resource
    std::map<std::string, Resource> table_;

    std::vector<Resource> recur_walk(std::filesystem::path p);
    std::string mapping(std::string uri) const;

public:
    ResourceManagerBaseFilesystem(/* args */) {}
    ResourceManagerBaseFilesystem(std::string path) : rsHome(path) {}
    virtual ~ResourceManagerBaseFilesystem() {}

    std::vector<Resource> idx() override;

    const Resource &query(std::string uri) const override;

    bool validRes(std::string uri, std::string hash) const override;

    std::vector<Resource> need_to_sync(std::vector<Resource> peer_table) const override;

    bool save(std::string uri, void *data, uint64_t offset, uint64_t data_len) override;

    std::optional<std::shared_ptr<uint8_t[]>> readFrom(std::string uri, const Block &blk) const override;
};

struct ResourceSerializerDto
{
    std::shared_ptr<uint8_t[]> data;
    uint64_t size;
};

class ResourceSerializer
{
public:
    ResourceSerializer(/* args */) = delete;
    ~ResourceSerializer() = delete;

    static ResourceSerializerDto serialize(const std::vector<Resource> &table);
    static std::vector<Resource> deserialize(uint8_t *data, uint64_t size);
};

#endif