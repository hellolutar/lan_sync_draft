#ifndef __RESOURCE_MANAGER_H_
#define __RESOURCE_MANAGER_H_

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <filesystem>

#include "exc/not_found_exc.h"

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

    bool operator==(const Resource &r) const;
};

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
};

class ResourceManagerBaseFilesystem : public ResourceManager
{
private:
    std::string rsHome;
    // mapping for uri and resource
    std::map<std::string, Resource> table_;

    std::vector<Resource> recur_walk(std::filesystem::path p);
    std::string mapping(std::string uri);

public:
    ResourceManagerBaseFilesystem(/* args */) {}
    ResourceManagerBaseFilesystem(std::string path) : rsHome(path) {}
    virtual ~ResourceManagerBaseFilesystem() {}

    std::vector<Resource> idx() override;

    const Resource &query(std::string uri) const override;

    bool validRes(std::string uri, std::string hash) const override;

    std::vector<Resource> need_to_sync(std::vector<Resource> peer_table) const override;

    bool save(std::string uri, void *data, uint64_t offset, uint64_t data_len) override;
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