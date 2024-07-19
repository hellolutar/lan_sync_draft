#ifndef __LOGIC_H_
#define __LOGIC_H_

#include <memory>

#include "framework/dep/net_addr.h"
#include "framework/dep/outputstream.h"

class Logic
{
public:
    Logic(/* args */) {}
    virtual ~Logic() {}

    /**
     * @brief defined by user
     *
     * @param data
     * @param data_len
     * @return const uint64_t
     */
    virtual const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t data_len) const = 0;

    /**
     * @brief defined by user
     *
     * @param peer
     * @param data
     * @param size
     */
    virtual void recv(const NetAddr &peer, std::shared_ptr<uint8_t[]> data, uint64_t size) = 0;
};

class LogicWrite : public Logic
{
private:
    std::shared_ptr<OutputStream> os_;

protected:
    void close()
    {
        auto c = os_.use_count();
        os_ = nullptr;
    }

public:
    LogicWrite(/* args */) {}
    virtual ~LogicWrite() {}

    /**
     * @brief called by user
     *
     * @param peer
     * @param data
     * @param size
     */
    virtual void write(std::shared_ptr<uint8_t[]> data, uint64_t size)
    {
        if (os_ != nullptr)
            os_->write(data, size);
    };

    /**
     * @brief Set the Output Stream object, called by the net_framekwork
     *
     * @param os
     */
    void setOutputStream(std::shared_ptr<OutputStream> os)
    {
        os_ = os;
    }
};

#endif