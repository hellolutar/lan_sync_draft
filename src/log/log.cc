#include "log.h"

void _append_timestamp(std::stringstream &ss)
{
    auto now = std::chrono::system_clock::now();

    std::time_t tt = std::chrono::system_clock::to_time_t(now);

    auto ptm = std::localtime(&tt);
    ss << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
}

void _format_log(std::string format, std::initializer_list<std::string> args)
{
    int last_idx = 0;
    for (auto &&s : args)
    {
        last_idx = format.find_first_of("{}", last_idx);
        format = format.replace(last_idx, 2, s);
    }
    std::cout << format << std::endl;
}