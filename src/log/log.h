#ifndef __LANSYNC_LOG_H_
#define __LANSYNC_LOG_H_

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <vector>

template <typename S>
void _log(S &ss)
{
}

template <typename S, typename A, typename... Args>
void _log(S &ss, A a, Args... arg)
{
    ss << a << " ";
    _log(ss, arg...);
}

void _append_timestamp(std::stringstream &ss);

template <typename... Args>
void _append_flag(std::stringstream &ss, std::string &flag, Args... arg)
{
    _append_timestamp(ss);
    ss << flag;

    _log(ss, arg...);

    std::cout << ss.str() << std::endl;
}

template <typename... Args>
void _error_log(Args... arg)
{
    std::stringstream ss;
    std::string flag(" [ERROR] ");
    _append_flag(ss, flag, arg...);
}

template <typename... Args>
void _warn_log(Args... arg)
{
    std::stringstream ss;
    std::string flag(" [WARN] ");
    _append_flag(ss, flag, arg...);
}

template <typename... Args>
void _info_log(Args... arg)
{
    std::stringstream ss;
    std::string flag(" [INFO] ");
    _append_flag(ss, flag, arg...);
}

template <typename... Args>
void _debug_log(Args... arg)
{
    std::stringstream ss;
    std::string flag(" [DEBUG] ");
    _append_flag(ss, flag, arg...);
}

template <typename... Args>
void _format_log(std::string &format, Args... args)
{
    std::vector<std::string> v;
    std::stringstream ss;

    auto concat_str = [&v, &ss](auto a)
    { ss<< a;  v.push_back(ss.str()); ss.str(""); };

    (..., concat_str(args));

    int last_idx = 0;
    for (auto &&s : v)
    {
        last_idx = format.find_first_of("{}", last_idx);
        format = format.replace(last_idx, 2, s);
    }
    std::cout << format << std::endl;
}

template <typename... Args>
void _prefix_format_log(std::string &tip, std::string &format, Args... args)
{
    std::stringstream ss;
    _append_timestamp(ss);
    ss << " " << tip;
    ss << format;
    format = ss.str();
    _format_log(format, args...);
}

template <typename... Args>
void ERROR_F(std::string format, Args... args)
{
    std::string tip = "[ERROR] ";
    _prefix_format_log(tip, format, args...);
}

template <typename... Args>
void WARN_F(std::string format, Args... args)
{
    std::string tip = "[WARN] ";
    _prefix_format_log(tip, format, args...);
}

template <typename... Args>
void INFO_F(std::string format, Args... args)
{
    std::string tip = "[INFO] ";
    _prefix_format_log(tip, format, args...);
}

template <typename... Args>
void DEBUG_F(std::string format, Args... args)
{
    std::string tip = "[DEBUG] ";
    _prefix_format_log(tip, format, args...);
}

#endif

#ifndef __LANSYNC_LOG_MACRO_
#define __LANSYNC_LOG_MACRO_

#define ERROR(...) _error_log(__VA_ARGS__)
#define WARN(...) _warn_log(__VA_ARGS__)
#define INFO(...) _info_log(__VA_ARGS__)
#define DEBUG(...) _debug_log(__VA_ARGS__)

#endif