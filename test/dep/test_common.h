#ifndef __LANSYNC_TEST_COMMON_H_
#define __LANSYNC_TEST_COMMON_H_

#include <string>
#include <sstream>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <cstring>

std::string genStr(uint32_t size);

std::shared_ptr<uint8_t[]> gen_u8_array(uint64_t size);

#endif