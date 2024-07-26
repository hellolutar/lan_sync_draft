#ifndef __EXAMPLE_EXAMPLE_COMMON_H_
#define __EXAMPLE_EXAMPLE_COMMON_H_

#include <memory>
#include <string>
#include <cstring>

std::shared_ptr<uint8_t[]> str_to_uint8(std::string content)
{
    std::shared_ptr<uint8_t[]> data(new uint8_t[content.size()]());
    memcpy(data.get(), content.data(), content.size());

    return data;
}

#endif