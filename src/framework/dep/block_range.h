#ifndef __LAN_SYNC_BLOCK_RANGE_H_
#define __LAN_SYNC_BLOCK_RANGE_H_

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

    const std::string str() const;
};

class Range
{
private:
    std::uint64_t start_;
    std::uint64_t end_;

public:
    Range(std::uint64_t start, std::uint64_t end) : start_(start), end_(end){};
    Range(std::string str);
    ~Range();

    bool operator==(const Range &r) const;
    bool operator<(const Range &r) const;

    const uint64_t size() const;
    std::string to_string() const;
    std::string to_string();

    uint64_t getStart() const;
    uint64_t getEnd() const;

    static const std::string defaultStr;
};

#endif