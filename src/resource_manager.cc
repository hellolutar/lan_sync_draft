#include "resource_manager.h"

#include <cstdlib>
#include <cstring>

#include "buf/buf_base_on_event.h"
#include "utils/openssl_utils.h"
#include "log/log.h"

using namespace std;

std::optional<Resource> ResourceManagerBaseFilesystem::query(const string &uri) const
{
    if (table_.size() == 0)
        const_cast<ResourceManagerBaseFilesystem &>(*this).idx();

    auto iter = table_.find(uri);
    if (iter == table_.end())
        return nullopt;

    return make_optional<Resource>(iter->second);
}

vector<Resource> ResourceManagerBaseFilesystem::idx()
{
    auto rss = recur_walk({rsHome});
    for (auto &&r : rss)
        table_[r.getUri()] = r;
    return std::move(rss);
}

vector<Resource> ResourceManagerBaseFilesystem::recur_walk(filesystem::path p)
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

        string hashRet = OpensslUtil::mdEncodeWithSHA3_512(pstr);
        Resource r{filename, uri, path, hashRet, size};
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

bool ResourceManagerBaseFilesystem::validRes(const string &uri, const string &hash) const
{
    const_cast<ResourceManagerBaseFilesystem &>(*this).idx();

    auto rs_opt = query(uri);
    if (rs_opt.has_value())
    {
        DEBUG_F("ResourceManagerBaseFilesystem::validRes():[{}]\n\t[{}]  \n\t[{}]", uri, hash, rs_opt.value().getHash());
        return hash.compare(rs_opt.value().getHash()) == 0;
    }
    else
    {
        WARN_F("ResourceManagerBaseFilesystem::validRes(): [{}] is not found in local filesystem!", uri);
        return false;
    }
}

std::vector<Resource> ResourceManagerBaseFilesystem::need_to_sync(std::vector<Resource> peer_table) const
{
    map<string, Resource> filtered;

    // 将peer_table全部放入filtered， 后续采用过滤的方法，将peer_table中过时的不需要同步的资源还是删除
    for (size_t i = 0; i < peer_table.size(); i++)
    {
        if (peer_table[i].getUri().size() == 0)
            continue;

        filtered[peer_table[i].getUri()] = peer_table[i];
    }

    if (filtered.size() == 0)
        return {};

    // 将filtered与local_table进行比较，进行滤除
    vector<Resource> local_table = const_cast<ResourceManagerBaseFilesystem &>(*this).idx();
    for (size_t i = 0; i < local_table.size(); i++)
    {
        Resource local_rs = local_table[i];
        Resource rs = filtered[local_rs.getUri()];
        if (rs.getUri().size() == 0 || rs.getSize() < local_rs.getSize())
        {
            // I have the resource or my resource should sync to peer.
            filtered.erase(local_rs.getUri());
            continue;
        }
        else if (rs.getSize() == local_rs.getSize())
        {
            if (rs.getHash() == local_rs.getHash())
                filtered.erase(local_rs.getUri());
            // else
            // LOG_DEBUG("ResourceManagerBaseFilesystem::cmpThenRetNeedToSyncTable() : uri[{}] need to add to synctable, reason:{}", rs.uri, "hash is not eq!");
        }
        // else
        // LOG_DEBUG("ResourceManagerBaseFilesystem::cmpThenRetNeedToSyncTable() : uri[{}] need to add to synctable, reason:{}", rs.uri, "size: peer[{}] > local[{}]", rs.getSize(), local_rs.getSize());
    }

    vector<Resource> want_to_sync;
    for (auto iter = filtered.begin(); iter != filtered.end(); iter++)
        want_to_sync.push_back(iter->second);

    return want_to_sync;
}

string ResourceManagerBaseFilesystem::mapping(const string &uri) const
{
    return rsHome + uri;
}

bool ResourceManagerBaseFilesystem::save(const string &uri, std::shared_ptr<uint8_t[]> data, uint64_t offset, uint64_t data_len)
{
    string pathstr = mapping(uri);

    auto path = filesystem::path(pathstr);
    auto ppath = filesystem::absolute(path).parent_path();
    if (!filesystem::exists(ppath))
        filesystem::create_directories(path.parent_path());

    auto peg = PersistFramework::getEngine();
    if (peg->saveTo(path.string(), offset, data, data_len) == data_len)
    {
        DEBUG_F("ResourceManagerBaseFilesystem::save() :URI:{} offset:{} data_len:{} SUCCESS ", uri, offset, data_len);
        return true;
    }
    DEBUG_F("ResourceManagerBaseFilesystem::save() : URI:{} offset:{} data_len:{} FAIL", uri, offset, data_len);
    return false;
}

std::optional<std::shared_ptr<uint8_t[]>> ResourceManagerBaseFilesystem::readFrom(std::string uri, const Block &blk) const
{
    auto eg = PersistFramework::getEngine();
    auto path = mapping(uri);

    uint64_t actual_read_size = 0;
    auto data = eg->readFrom(path, blk.start, blk.size(), actual_read_size);
    // todo(20240715, lutar) when the actual_read_size is not equal the blk.size()
    return data;
}

/**
 * 2bit位 ： 00 为uint8_t，01为uint16_t, 10 uint32_t
 */
uint8_t set_flag_and_accum_hdsize(std::string str, BufBaseonEvent &buf, uint16_t &hd_size)
{
    if (str.size() <= 255)
    {
        hd_size += sizeof(uint8_t);
        uint8_t tmp_size = str.size();
        buf.add(&tmp_size, sizeof(uint8_t));
        return 0;
    }
    else if (str.size() <= 65535)
    {
        hd_size += sizeof(uint16_t);
        uint16_t tmp_size = str.size();
        buf.add(reinterpret_cast<uint8_t *>(&tmp_size), sizeof(uint16_t));
        return 1;
    }
    else
    {
        hd_size += sizeof(uint32_t);
        uint32_t tmp_size = str.size();
        buf.add(reinterpret_cast<uint8_t *>(&tmp_size), sizeof(uint32_t));
        return 2;
    }
}
ResourceSerializerDto ResourceSerializer::serialize(const std::vector<Resource> &table)
{
    if (table.size() == 0)
        return {nullptr, 0};

    BufBaseonEvent buf;
    for (auto &&r : table)
    {
        uint16_t hd_size = sizeof(uint8_t);
        uint8_t flag = 0;

        BufBaseonEvent tmp_buf;
        flag |= set_flag_and_accum_hdsize(r.getName(), tmp_buf, hd_size);

        flag <<= 2;
        flag |= set_flag_and_accum_hdsize(r.getUri(), tmp_buf, hd_size);

        flag <<= 2;
        flag |= set_flag_and_accum_hdsize(r.getPath(), tmp_buf, hd_size);

        flag <<= 2;
        flag |= set_flag_and_accum_hdsize(r.getHash(), tmp_buf, hd_size);

        buf.add(&flag, sizeof(uint8_t));

        auto size_array = tmp_buf.data().get();
        buf.add(size_array, tmp_buf.size());
        uint64_t tmp_r_szie = r.getSize();
        buf.add(reinterpret_cast<uint8_t *>(&tmp_r_szie), sizeof(uint64_t));

        buf.add(reinterpret_cast<uint8_t *>(const_cast<char *>(r.getName().data())), r.getName().size());
        buf.add(reinterpret_cast<uint8_t *>(const_cast<char *>(r.getUri().data())), r.getUri().size());
        buf.add(reinterpret_cast<uint8_t *>(const_cast<char *>(r.getPath().data())), r.getPath().size());
        buf.add(reinterpret_cast<uint8_t *>(const_cast<char *>(r.getHash().data())), r.getHash().size());
    }
    return {buf.data(), buf.size()};
}

uint64_t readSize(uint8_t flag, uint8_t *data, uint8_t &step)
{
    if (flag == 0)
    {
        uint8_t size = *data;
        step = 1;
        return size;
    }
    else if (flag == 1)
    {
        uint16_t size = *(reinterpret_cast<uint16_t *>(data));
        step = 2;
        return size;
    }
    else
    {
        uint32_t size = *(reinterpret_cast<uint32_t *>(data));
        step = 4;
        return size;
    }
}

std::vector<Resource> ResourceSerializer::deserialize(std::shared_ptr<uint8_t[]> data, uint64_t size)
{
    vector<Resource> rs;
    auto datap = data.get();
    uint64_t pos = 0;
    const uint32_t loop_times_limit = 4294967295;
    uint32_t loop_time = 0;
    while (pos < size && loop_time < loop_times_limit)
    {
        loop_time++;

        // 读取flag
        uint8_t flag = *datap;
        // 读取size
        uint8_t name_flag = (flag & 0b11000000) >> 6;
        uint8_t uri_flag = (flag & 0b00110000) >> 4;
        uint8_t path_flag = (flag & 0b00001100) >> 2;
        uint8_t hash_flag = flag & 0b00000011;
        // 读取内容
        datap++;
        pos += sizeof(uint8_t);

        // step 为 name_size 的类型的占用的存储空间的大小
        uint8_t step = 0;
        auto name_size = readSize(name_flag, datap, step);
        pos += step;

        datap += step;
        auto uri_size = readSize(uri_flag, datap, step);
        pos += step;

        datap += step;
        auto path_size = readSize(path_flag, datap, step);
        pos += step;

        datap += step;
        auto hash_size = readSize(hash_flag, datap, step);
        pos += step;

        datap += step;
        uint64_t rs_size = *(reinterpret_cast<uint64_t *>(datap));
        // Resource.size 固定为uint64_t, 故不分配空间记录它的大小，故pos不需要累加step

        datap += 8;

        // 读取string
        auto name = string(reinterpret_cast<char *>(datap), name_size);
        datap += name_size;
        pos += name_size;

        auto uri = string(reinterpret_cast<char *>(datap), uri_size);
        datap += uri_size;
        pos += uri_size;

        auto path = string(reinterpret_cast<char *>(datap), path_size);
        datap += path_size;
        pos += path_size;

        auto hash = string(reinterpret_cast<char *>(datap), hash_size);
        datap += hash_size;
        pos += hash_size;

        pos += 8; // 8 为Resource.size的类型的存储空间大小，因为是类型是uint64_t，故+8

        rs.push_back({name, uri, path, hash, rs_size});
    }

    return rs;
}
