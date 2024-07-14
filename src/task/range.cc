#include "task/range.h"
#include <sstream>

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
 std::string Range::to_string(){
    std::stringstream ss;
    ss << start_ << FLAG_XHEADER_CONTENT_BETWEEN;
    if (size() != 0)
    {
        ss << (start_ + size());
    }

    return ss.str();
 }

 const std::string Range::defaultStr = "0-0/0/last";