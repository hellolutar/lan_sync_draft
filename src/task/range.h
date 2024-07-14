#ifndef __RANGE_H_
#define __RANGE_H_

#include <string>
#include <cstdint>

#include "constants/proto_constants.h"


class Range
{
private:
    std::uint64_t start_;
    std::uint64_t end_;

public:
    Range(std::uint64_t start, std::uint64_t end) : start_(start), end_(end){};
    ~Range();

    bool operator==(const Range &r) const;
    bool operator<(const Range &r) const;

    const uint64_t size() const;
    std::string to_string();

    static const std::string defaultStr;
};

#endif