#ifndef __LANSYNC_LOG_H_
#define __LANSYNC_LOG_H_

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <vector>

class FormatLog
{
private:
    uint8_t interval_ = 5;

public:
    FormatLog(int i) : interval_(i) {}
    ~FormatLog() {}

    std::string red(const std::string &str);
    std::string green(const std::string &str);
    std::string yellow(const std::string &str);
    std::string blue(const std::string &str);
    std::string gray(const std::string &str);

    std::string space(const std::string &, int i);
};

extern FormatLog formatlog_;

void _append_timestamp(std::stringstream &ss);

template <typename... Args>
void _format_log(std::string &prefix, const std::string &position, std::string &format, Args... args)
{
    std::vector<std::string> v;
    std::stringstream ss;

    auto concat_str = [&v, &ss](auto a)
    { ss<< a;  v.push_back(ss.str()); ss.str(""); };

    (..., concat_str(args));

    prefix += formatlog_.space(position, 40);

    int last_idx = 0;
    for (int64_t i = 0; i < v.size(); i++)
    {
        std::string s = v[i];
        last_idx = format.find_first_of("{}", last_idx);
        if (last_idx == std::string::npos)
            format += s;
        else
            format = format.replace(last_idx, 2, s);
    }

    // printf(format.data());
    std::cout << prefix << format << std::endl;
}

template <typename... Args>
void _prefix_format_log(std::string &tip, const std::string &position, std::string &format, Args... args)
{
    std::stringstream ss;
    _append_timestamp(ss);
    ss << " " << tip;
    auto prefix = ss.str();
    _format_log(prefix, position, format, args...);
}

template <typename... Args>
void ERROR_F(std::string position, std::string format, Args... args)
{
    std::string tip = formatlog_.red("[ERROR]");
    _prefix_format_log(tip, position, format, args...);
}

template <typename... Args>
void WARN_F(std::string position, std::string format, Args... args)
{
    std::string tip = formatlog_.yellow("[WARN]");
    _prefix_format_log(tip, position, format, args...);
}

template <typename... Args>
void INFO_F(std::string position, std::string format, Args... args)
{
    std::string tip = formatlog_.green("[INFO]");
    _prefix_format_log(tip, position, format, args...);
}

template <typename... Args>
void DEBUG_F(std::string position, std::string format, Args... args)
{
    std::string tip = formatlog_.blue("[DEBUG]");
    _prefix_format_log(tip, position, format, args...);
}

#endif

#ifndef __LANSYNC_LOG_MACRO_
#define __LANSYNC_LOG_MACRO_

#endif