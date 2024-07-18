#include "log.h"

void _append_timestamp(std::stringstream &ss)
{
    auto now = std::chrono::system_clock::now();

    std::time_t tt = std::chrono::system_clock::to_time_t(now);

    auto ptm = std::localtime(&tt);
    ss << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
}