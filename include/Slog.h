//
// Created by sphdx.
//

#ifndef SLOG_H
#define SLOG_H

#include <iostream>
#include <ostream>
#include <string>
#include <chrono>
#include <iomanip>
#include "app/ui/Log.h"
#include "utils/Utils.h"

#ifdef SLOG_ENABLED
    inline constexpr bool on = true;
#else
inline constexpr bool on = false;
#endif

template<typename T>
concept Container = requires(T t)
{
    t.begin();
    t.end();
} && !std::is_same_v<T, std::string>;

class Slog {
    Slog() = default;

    template<typename Key, typename Val>
    class LoggerOption {
    public:
        const Key key_;
        const Val value_;

        LoggerOption(Key &&key, Val &&val)
            : key_(std::forward<Key>(key)), value_(std::forward<Val>(val)) {
        }
    };

    template<typename... Keys, typename... Vals>
    static void log(const std::string &type, const std::string &message, const LoggerOption<Keys, Vals> &... args);

    template<typename T>
    static std::string sl_print(const T &val);

    template<typename A, typename B>
    static std::string sl_print(const std::pair<A, B> &p);

    template<Container C>
    static std::string sl_print(const C &container);

public:
    Slog(const Slog &) = delete;

    Slog &operator=(const Slog &) = delete;

    template<typename... Keys, typename... Vals>
    static void info(const std::string &message, const LoggerOption<Keys, Vals> &... args);

    template<typename... Keys, typename... Vals>
    static void warn(const std::string &message, const LoggerOption<Keys, Vals> &... args);

    template<typename... Keys, typename... Vals>
    static void error(const std::string &message, const LoggerOption<Keys, Vals> &... args);

    template<typename T>
    static void array(T* arr, size_t& size, char delim = ',');

    template<typename Key, typename Val>
    static LoggerOption<Key, Val> opt(Key &&key, Val &&val);
};

template<typename... Keys, typename... Vals>
void Slog::info(const std::string &message, const LoggerOption<Keys, Vals> &... args) {
    if constexpr (on) log("ИНФО", message, args...);
}

template<typename... Keys, typename... Vals>
void Slog::warn(const std::string &message, const LoggerOption<Keys, Vals> &... args) {
    if constexpr (on) log("ПРЕДУПРЕЖДЕНИЕ", message, args...);;
}

template<typename... Keys, typename... Vals>
void Slog::error(const std::string &message, const LoggerOption<Keys, Vals> &... args) {
    if constexpr (on) log("ОШИБКА", message, args...);
}

template<typename T>
void Slog::array(T *arr, size_t &size, char delim) {
    if constexpr (!on) return;

    std::ostringstream oss;

    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // "%Y-%m-%d %H:%M:%S"
    oss << std::put_time(std::localtime(&now), "%H:%M:%S") << ' ';

    oss << "[" << "МАССИВ" << "] " << '\n';

    utils::print_array(arr, size, delim);

    const std::string log_line = oss.str();

    std::cout << log_line << std::endl;

    app::push_gui_log(log_line);
}

template<typename Key, typename Val>
Slog::LoggerOption<Key, Val> Slog::opt(Key &&key, Val &&val) {
    return LoggerOption<Key, Val>(std::forward<Key>(key), std::forward<Val>(val));
}

template<typename... Keys, typename... Vals>
void Slog::log(const std::string &type, const std::string &message, const LoggerOption<Keys, Vals> &... args) {
    std::ostringstream oss;

    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // "%Y-%m-%d %H:%M:%S"
    oss << std::put_time(std::localtime(&now), "%H:%M:%S") << ' ';

    oss << "[" << type << "] " << message;

    ((oss << ", " << args.key_ << '=' << sl_print(args.value_)), ...);

    const std::string log_line = oss.str();

    std::cout << log_line << std::endl;

    app::push_gui_log(log_line);
}

template<typename T>
std::string Slog::sl_print(const T &val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

template<typename A, typename B>
std::string Slog::sl_print(const std::pair<A, B> &p) {
    std::ostringstream oss;

    oss << '(';
    sl_print(p.first);
    oss << ", ";
    sl_print(p.second);
    oss << ')';

    return oss.str();
}

template<Container C>
std::string Slog::sl_print(const C &container) {
    std::ostringstream oss;

    oss << '[';
    auto it = container.begin();
    if (it != container.end()) {
        sl_print(*it);
        ++it;
    }
    for (; it != container.end(); ++it) {
        oss << ", ";
        sl_print(*it);
    }
    oss << ']';

    return oss.str();
}

#endif //SLOG_H
