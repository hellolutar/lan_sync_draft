#ifndef __LANSYNC_LOG_H_
#define __LANSYNC_LOG_H_

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
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

#endif

#ifndef __LANSYNC_LOG_MACRO_
#define __LANSYNC_LOG_MACRO_

#define ERROR(...) _error_log(__VA_ARGS__)
#define WARN(...) _warn_log(__VA_ARGS__)
#define INFO(...) _info_log(__VA_ARGS__)
#define DEBUG(...) _debug_log(__VA_ARGS__)

#endif