//
// Created by JellyfishKnight on 25-3-3.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QPlainTextEdit>
#include <QDateTime>
#include <mutex>


class QString;

enum LogLevel
{
    debug,
    info,
    warn,
    error
};

void init_logger(QPlainTextEdit* log_window);

class Logger {
public:
    static void log(const std::string& message, LogLevel level);

private:
    Logger(QPlainTextEdit* log_window) : log_window(log_window) {}

    ~Logger() = default;

    inline static Logger* pthis;

    friend void init_logger(QPlainTextEdit* log_window);

    QPlainTextEdit* log_window;
    std::mutex log_mutex;
};

constexpr std::string DEBUG_STR = "DEBUG: ";
constexpr std::string INFO_STR = "INFO: ";
constexpr std::string WARN_STR = "WARN: ";
constexpr std::string ERROR_STR = "ERROR: ";

#define LOG_DEBUG(message) Logger::log(DEBUG_STR + message, LogLevel::debug);
#define LOG_INFO(message) Logger::log(INFO_STR + message, LogLevel::info)
#define LOG_WARN(message) Logger::log(WARN_STR + message, LogLevel::warn)
#define LOG_ERROR(message) Logger::log(ERROR_STR + message, LogLevel::error)

#endif //LOGGER_HPP
