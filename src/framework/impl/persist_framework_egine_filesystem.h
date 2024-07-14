#ifndef __PERSIST_FRAMEWORK_EGINE_IMPL_WITH_FILESYSTEM_H_
#define __PERSIST_FRAMEWORK_EGINE_IMPL_WITH_FILESYSTEM_H_

#include <sys/fcntl.h>
#include <malloc.h>
#include <unistd.h>

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>

#include "constants/constants.h"
#include "framework/itf/persist/persist_framework.h"

class PersistFrameworkEgineImplWithFileSystem: public PersistFrameworkEgine
{

public:
    PersistFrameworkEgineImplWithFileSystem(/* args */);
    virtual ~PersistFrameworkEgineImplWithFileSystem();

    std::shared_ptr<uint8_t[]> readFrom(std::string path, uint64_t &ret_len) override;
    std::shared_ptr<uint8_t[]> readFrom(std::string path, uint64_t offset, uint64_t size, uint64_t &ret_len) override;

    uint64_t saveTo(std::string path, uint64_t offset, uint8_t *data, uint64_t size) override;
    bool mkdirs(std::string path);
};

#endif
