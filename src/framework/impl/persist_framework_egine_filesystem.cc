#include "persist_framework_egine_filesystem.h"

using namespace std;

PersistFrameworkEgineImplWithFileSystem::PersistFrameworkEgineImplWithFileSystem(/* args */)
{
}

PersistFrameworkEgineImplWithFileSystem::~PersistFrameworkEgineImplWithFileSystem()
{
}

std::optional<std::shared_ptr<uint8_t[]>> PersistFrameworkEgineImplWithFileSystem::readFrom(string path, uint64_t &ret_len)
{
    filesystem::path p(path);
    size_t size = filesystem::file_size(p);

    return readFrom(path, 0, size, ret_len);
}

std::optional<std::shared_ptr<uint8_t[]>> PersistFrameworkEgineImplWithFileSystem::readFrom(string path, uint64_t offset, uint64_t size, uint64_t &ret_len)
{
    ret_len = 0;

    int fd = open(path.data(), O_RDONLY);
    if (fd < 0)
        return nullopt;

    off_t curpos = lseek(fd, offset, SEEK_SET);
    if (curpos != offset)
    {
        return nullopt;
    }

    filesystem::path filep(path);
    uint64_t end = min(offset + size, filesystem::file_size(filep));

    std::shared_ptr<uint8_t[]> retData(new uint8_t[size]());

    uint8_t *data = retData.get();
    uint8_t *data_pos = data;
    memset(data, 0, size);
    uint64_t once_write_max_num = ONCE_MAX_READ_SIZE;

    while (curpos < end)
    {
        uint64_t start_at = curpos;
        uint64_t remained = end - curpos;

        // 读取文件内容
        uint64_t expected_read = min(once_write_max_num, remained);
        uint64_t actual_read = read(fd, data_pos, expected_read);
        for (size_t i = 0; i < mons_.size(); i++)
        {
            auto &rdm = mons_[i];
            try
            {
                rdm->monitor(curpos, data_pos, actual_read);
            }
            catch (const std::exception &e)
            {
                // LOG_ERROR("PersistFrameworkEgineImplWithFileSystem::readSize() : reason:{}", e.what());
            }
        }
        curpos += actual_read;
        ret_len += actual_read;
        data_pos += actual_read;
    }
    close(fd);

    return make_optional<std::shared_ptr<uint8_t[]>>(retData);
}

uint64_t PersistFrameworkEgineImplWithFileSystem::saveTo(string path, uint64_t offset, uint8_t *data, uint64_t size)
{
    uint64_t ret_len = 0;
    int fd = open(path.data(), O_RDWR | O_CREAT, 0644);
    if (fd < 0)
        return ret_len;

    off_t curpos = lseek(fd, offset, SEEK_SET);

    uint64_t end = offset + size;
    uint64_t once_write_max_num = ONCE_MAX_READ_SIZE;

    uint64_t expected_write = min(once_write_max_num, size);
    uint64_t actual_write = write(fd, data, expected_write);
    curpos += actual_write;
    ret_len += actual_write;

    while (curpos < end)
    {
        uint64_t remained = end - curpos;

        expected_write = min(once_write_max_num, remained);
        actual_write = write(fd, data, expected_write);
        curpos += actual_write;
        ret_len += actual_write;
    }

    close(fd);

    return ret_len;
}

bool PersistFrameworkEgineImplWithFileSystem::mkdirs(std::string path)
{
    filesystem::path p(path);
    if (!filesystem::exists(p))
        return filesystem::create_directories(p);

    return false;
}

PersistMonitor::~PersistMonitor()
{
}

PersistMonitor::PersistMonitor()
{
}