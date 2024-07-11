#include "resource_manager.h"

#include <cstdlib>

using namespace std;

const std::string &Resource::getName() const
{
    return name_;
}
const std::string &Resource::getUri() const
{
    return uri_;
}
const std::string &Resource::getPath() const
{
    return path_;
}
const std::string &Resource::getHash() const
{
    return hash_;
}
const std::uint64_t Resource::getSize() const
{
    return size_;
}

ResourceManager::~ResourceManager()
{
}

const Resource ResourceManager::queryByUri(string uri)
{
    if (table_.size() == 0)
        idx();

    auto iter = table_.find(uri);
    if (iter == table_.end())
        throw NotFoundException("ResourceManager::queryByUri()! uri:", uri);

    return iter->second;
}

vector<Resource> ResourceManager::idx()
{
    // filesystem::path p(rsHome);
    auto rss = recur_walk({rsHome});
    for (auto &&r : rss)
        table_[r.getUri()] = r;
    return rss;
}

vector<Resource> ResourceManager::recur_walk(filesystem::path p)
{

    if (filesystem::is_regular_file(p))
    {
        string pstr = p.string();
        string filename = p.filename();
        string path = filesystem::absolute(p).string();
        uint64_t size = filesystem::file_size(p);
        filesystem::file_time_type fileLastWriteTime = filesystem::last_write_time(p);

        int index = path.find(rsHome) + rsHome.size();
        string uri = path.substr(index);

        // string hashRet = OpensslUtil::mdEncodeWithSHA3_512(pstr);
        // if (hashRet.size() == 0)
        //     return {};

        Resource r{filename, path, path, "", size};
        return {r};
    }

    if (filesystem::is_directory(p))
    {
        vector<Resource> paths;
        for (auto &&i : filesystem::directory_iterator(p))
        {
            auto ret = recur_walk(i);
            paths.insert(paths.begin(), ret.begin(), ret.end());
        }
        return paths;
    }
    return {};
}

bool ResourceManager::validRes(string uri, string hash)
{
    const Resource rs = queryByUri(uri);

    if (hash.compare(rs.getHash()) == 0)
    {
        return true;
    }
    else
        return false;
}

std::vector<struct Resource> ResourceManager::diff_for_need_to_sync(std::vector<Resource> peer_table)
{
    map<string, Resource> filtered;

    for (size_t i = 0; i < peer_table.size(); i++)
    {
        if (peer_table[i].getUri().size())
            continue;

        filtered[peer_table[i].getName()] = peer_table[i];
    }

    if (filtered.size() == 0)
        return {};

    vector<Resource> local_table = idx();
    for (size_t i = 0; i < local_table.size(); i++)
    {
        Resource local_rs = local_table[i];
        struct Resource rs = filtered[local_rs.getName()];
        if (rs.getName().size() == 0 || rs.getSize() < local_rs.getSize())
        {
            // I have the resource or my resource should sync to peer.
            filtered.erase(local_rs.getName());
            continue;
        }
        else if (rs.getSize() == local_rs.getSize())
        {
            if (rs.getHash() == local_rs.getHash())
                filtered.erase(local_rs.getName());
            // else
            // LOG_DEBUG("ResourceManager::cmpThenRetNeedToSyncTable() : uri[{}] need to add to synctable, reason:{}", rs.uri, "hash is not eq!");
        }
        // else
        // LOG_DEBUG("ResourceManager::cmpThenRetNeedToSyncTable() : uri[{}] need to add to synctable, reason:{}", rs.uri, "size: peer[{}] > local[{}]", rs.getSize(), local_rs.getSize());
    }

    vector<struct Resource> want_to_sync;
    for (auto iter = filtered.begin(); iter != filtered.end(); iter++)
        want_to_sync.push_back(iter->second);

    return want_to_sync;
}

string ResourceManager::mapping(string uri)
{
    return rsHome + uri;
}

bool ResourceManager::saveLocal(string uri, void *data, uint64_t offset, uint64_t data_len)
{
    // LOG_DEBUG("ResourceManager::saveLocal() : URI:{} offset:{} data_len:{}", uri, offset, data_len);
    string pathstr = mapping(uri);

    auto path = filesystem::path(pathstr);
    auto ppath = filesystem::absolute(path).parent_path();
    if (!filesystem::exists(ppath))
    {
        filesystem::create_directories(path.parent_path());
    }

    // uint64_t ret = io.writeFile(pathstr, offset, data, data_len);
    // if (ret < 0)
    //     return false;

    return true;
}
