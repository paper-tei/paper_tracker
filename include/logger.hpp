//
// Created by JellyfishKnight on 25-3-3.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QPlainTextEdit>
#include <QDateTime>
#include <mutex>
#include <iostream>


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

    static void log(const QString& message, LogLevel level) {
        if (message.isNull() || message.isEmpty())
        {
            return ;
        }
        if (pthis->log_window->document()->blockCount() > 1000)
        {
            // 清空日志窗口
            QMetaObject::invokeMethod(pthis->log_window, "clear",
                          Qt::QueuedConnection);
        }
        std::lock_guard<std::mutex> lock(pthis->log_mutex);
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
        QString log_message = QString("[%1] %2").arg(current_date).arg(message);
        if (level != debug)
        {
            QMetaObject::invokeMethod(pthis->log_window, "appendPlainText",
                          Qt::QueuedConnection, Q_ARG(QString, log_message));
        }
#ifdef DEBUG
        std::cout << log_message.toStdString() << std::endl;
#endif
    }


private:
    Logger(QPlainTextEdit* log_window) : log_window(log_window) {}

    ~Logger() = default;

    inline static Logger* pthis;

    friend void init_logger(QPlainTextEdit* log_window);

    QPlainTextEdit* log_window;
    std::mutex log_mutex;
};

inline void init_logger(QPlainTextEdit* log_window)
{
    if (Logger::pthis == nullptr)
    {
        Logger::pthis = new Logger(log_window);
    }
}

#define LOG_DEBUG(message) Logger::log(QString("DEBUG: %1").arg(message), LogLevel::debug);
#define LOG_INFO(message) Logger::log(QString("INFO: %1").arg(message), LogLevel::info)
#define LOG_WARN(message) Logger::log(QString("WARN: %1").arg(message), LogLevel::warn)
#define LOG_ERROR(message) Logger::log(QString("ERROR: %1").arg(message), LogLevel::error)


#endif //LOGGER_HPP
