#ifndef __LOGIC_H_
#define __LOGIC_H_

#include <memory>

#include "framework/dep/net_addr.h"
#include "framework/dep/outputstream.h"

class NetAbilityContext
{
protected:
    std::shared_ptr<OutputStream> os_ = nullptr;
    NetAddr from_;

public:
    NetAbilityContext(/* args */) {}
    NetAbilityContext(std::shared_ptr<OutputStream> os, const NetAddr from) : os_(std::move(os)), from_(from) {}
    ~NetAbilityContext() {}

    virtual void write(std::shared_ptr<uint8_t[]> data, uint64_t size);
    virtual const NetAddr& from() const;
};

class Logic
{
public:
    Logic(/* args */) {}
    virtual ~Logic() {}

    /**
     * @brief defined by user
     *
     * @param peer
     * @param data
     * @param size
     */
    virtual void recv(NetAbilityContext &ctx, std::shared_ptr<uint8_t[]> data, uint64_t size) = 0;
};

class LogicProto : public Logic
{
public:
    LogicProto(/* args */) {}
    ~LogicProto() {}

    virtual const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t data_len) const;
};

class LogicWrite : public LogicProto
{
private:
    std::unique_ptr<OutputStream> os_;

protected:
    /**
     * release the outputstream
     */
    void close()
    {
        os_ = nullptr;
    }

public:
    LogicWrite(/* args */) {}
    virtual ~LogicWrite() {}

    virtual const uint64_t isExtraAllDataNow(std::shared_ptr<uint8_t[]> data, uint64_t data_len) const override
    {
        return data_len;
    };

    /**
     * @brief Set the Output Stream object, called by the net_framekwork
     *
     * @param os
     */
    void setOutputStream(std::unique_ptr<OutputStream> &&os)
    {
        os_ = std::move(os);
    }

    const std::unique_ptr<OutputStream> &getOutputStream()
    {
        return os_;
    }
};

#endif