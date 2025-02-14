#include "block_range.h"

#include <sstream>

const bool Block::operator==(const Block &other)
{
    return static_cast<const Block &>(*this) == other;
}

const bool Block::operator==(const Block &other) const
{
    return start == other.start && end == other.end;
}

const uint64_t Block::pos(uint64_t offset)
{
    if (offset < BLOCK_SIZE)
        return 0;

    uint64_t pos = offset / BLOCK_SIZE;

    return pos;
}

const uint64_t Block::bitPos(uint64_t pos)
{
    return pos * BLOCK_SIZE;
}

const std::string Block::str() const
{
    return "[" + std::to_string(start) + ", " + std::to_string(end) + ")";
}

const uint64_t Block::size() const
{
    return end - start;
}

Range::Range(std::string str)
{
    uint64_t pos = str.find_first_of("-");
    start_ = std::stoi(str.substr(0, pos));
    end_ = std::stoi(str.substr(pos + 1));
}

Range::~Range()
{
}

bool Range::operator==(const Range &r) const
{
    return r.start_ == start_ && r.end_ == end_;
}

bool Range::operator<(const Range &r) const
{
    if (r.start_ != start_)
        return false;

    return end_ < r.end_;
}

const uint64_t Range::size() const
{
    return end_ - start_;
}

// str like : 0-500
// str like : 0-
std::string Range::to_string()
{
    return static_cast<const Range &>(*this).to_string();
}

std::string Range::to_string() const
{
    std::stringstream ss;
    ss << start_ << FLAG_XHEADER_CONTENT_BETWEEN;
    if (size() != 0)
    {
        ss << (start_ + size());
    }

    return ss.str();
}

uint64_t Range::getStart() const
{
    return start_;
}

uint64_t Range::getEnd() const
{
    return end_;
}

const std::string Range::defaultStr = "0-0/0/last";