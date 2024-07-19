#ifndef __FRAMEWORK_DEP_CONNECTION_H_
#define __FRAMEWORK_DEP_CONNECTION_H_

#include "net_addr.h"

class Connection
{
protected:
    NetAddr addr_;

public:
    Connection(const NetAddr & a) : addr_(a) {}
    virtual ~Connection() {}

    bool isMe(const NetAddr &o)
    {
        return addr_.str() == o.str();
    }

    const NetAddr addr() const
    {
        return addr_;
    }
};

#endif