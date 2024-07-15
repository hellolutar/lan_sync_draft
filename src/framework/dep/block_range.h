#ifndef __LAN_SYNC_BLOCK_RANGE_H_
#define __LAN_SYNC_BLOCK_RANGE_H_

#include <string>
#include <cstdint>

#include "constants/constants.h"
#include "constants/proto_constants.h"

class Block
{
public:
    uint64_t start;
    uint64_t end;
    Block(){};
    Block(uint64_t start, uint64_t end) : start(start), end(end){};
    ~Block(){};

    const bool operator==(const Block &other);

    const bool operator==(const Block &other) const;

    bool operator<(const Block &other) const {
        return end < other.end;
    };

    const uint64_t size() const;

    static const uint64_t pos(uint64_t offset);
    static const uint64_t bitPos(uint64_t pos);
};

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