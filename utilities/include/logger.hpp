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
    static void log(LogLevel level, const std::string& message);

private:
    explicit Logger(QPlainTextEdit* log_window) : log_window(log_window) {}

    ~Logger() = default;

    inline static Logger* pthis;

    friend void init_logger(QPlainTextEdit* log_window);

    QPlainTextEdit* log_window;
    std::mutex log_mutex;
};

const std::string DEBUG_STR = "DEBUG: ";
const std::string INFO_STR = "INFO: ";
const std::string WARN_STR = "WARN: ";
const std::string ERROR_STR = "ERROR: ";

#define LOG_DEBUG(message) Logger::log(LogLevel::debug, DEBUG_STR + message);
#define LOG_INFO(message) Logger::log(LogLevel::info, INFO_STR + message)
#define LOG_WARN(message) Logger::log(LogLevel::warn, WARN_STR + message)
#define LOG_ERROR(message) Logger::log(LogLevel::error, ERROR_STR + message)

#endif //LOGGER_HPP
