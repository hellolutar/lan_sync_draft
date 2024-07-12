#ifndef __ABS_BUF_H_
#define __ABS_BUF_H_

#include <cstdint>
#include <memory>

class AbsBuf
{
protected:
    std::shared_ptr<uint8_t[]> d = nullptr;

public:
    AbsBuf(){};
    virtual ~AbsBuf();
    virtual uint64_t add(uint8_t *data, uint64_t len) = 0;
    virtual std::shared_ptr<uint8_t[]> data() = 0;
    virtual uint64_t size() = 0;
};

#endif