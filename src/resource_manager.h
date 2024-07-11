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
};

class ResourceManager
{
private:
    std::string rsHome;
    std::map<std::string, Resource> table_;

    std::vector<Resource> recur_walk(filesystem::path p);

public:
    ResourceManager(/* args */) {}
    ResourceManager(std::string path) : rsHome(path) {}
    ~ResourceManager() {}

    std::vector<Resource> idx();

    const Resource queryByUri(std::string uri);

    bool validRes(std::string uri, std::string hash);

    std::vector<struct Resource> diff_for_need_to_sync(std::vector<struct Resource> peer_table);

    std::string mapping(std::string uri);

    bool saveLocal(std::string uri, void *data, uint64_t offset, uint64_t data_len);
};

#endif