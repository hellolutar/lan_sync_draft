
#include "dep/test_common.h"

std::string dic = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

std::string genStr(uint32_t size)
{
    std::stringstream ss{};
    for (uint64_t i = 0; i < size;)
    {
        uint64_t subsize = std::min(static_cast<uint64_t>(size - i), dic.size());
        auto tmp = dic.substr(0, subsize);
        ss << tmp;
        i += subsize;
    }
    return ss.str();
}

std::shared_ptr<uint8_t[]> gen_u8_array(uint64_t size)
{
    std::shared_ptr<uint8_t[]> ret(new uint8_t[size]());
    auto str = genStr(size);
    memcpy(ret.get(), str.data(), size);
    return ret;
}
