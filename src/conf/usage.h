#ifndef __CONF_USAGE_FLAGS_H_
#define __CONF_USAGE_FLAGS_H_

#include <cstdint>

#include <gflags/gflags.h>

DECLARE_string(config_path);

class Usage
{
    int argc_;
    char **argv_;

public:
    Usage(int c, char **v) : argc_(c), argv_(v) {}
    ~Usage() {}

    void parse();
};

#endif