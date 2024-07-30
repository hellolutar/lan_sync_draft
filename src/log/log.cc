#include "log.h"

FormatLog formatlog_(8);

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

std::string FormatLog::red(const std::string &str)
{
    char buf[1024];
    std::string format_str = "\033[0m\033[;31m%-" + std::to_string(interval_) + "s\033[0m";
    sprintf(buf, format_str.data(), str.data());
    return std::string(buf);
}
std::string FormatLog::green(const std::string &str)
{
    char buf[1024];
    std::string format_str = "\033[0m\033[;32m%-" + std::to_string(interval_) + "s\033[0m";
    sprintf(buf, format_str.data(), str.data());
    return std::string(buf);
}
std::string FormatLog::yellow(const std::string &str)
{
    char buf[1024];
    std::string format_str = "\033[0m\033[;33m%-" + std::to_string(interval_) + "s\033[0m";
    sprintf(buf, format_str.data(), str.data());
    return std::string(buf);
}
std::string FormatLog::blue(const std::string &str)
{
    char buf[1024];
    std::string format_str = "\033[0m\033[;34m%-" + std::to_string(interval_) + "s\033[0m";
    sprintf(buf, format_str.data(), str.data());
    return std::string(buf);
}
std::string FormatLog::gray(const std::string &str)
{
    char buf[1024];
    std::string format_str = "\033[0m\033[;2m%-" + std::to_string(interval_) + "s\033[0m";
    sprintf(buf, format_str.data(), str.data());
    return std::string(buf);
}

std::string FormatLog::space(const std::string& str, int i)
{
    char buf[1024];
    std::string format_str =   "\033[0m\033[;2m%-" + std::to_string(i) + "s\033[0m";
    sprintf(buf, format_str.data(), str.data());
    return std::string(buf);
}
