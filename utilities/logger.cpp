//
// Created by JellyfishKnight on 25-3-4.
//
#include "logger.hpp"
#include "iostream"

void Logger::log(const std::string& message, LogLevel level)
{
    if (message.empty())
    {
        return ;
    }
    if (pthis->log_window->document()->blockCount() > 1000)
    {
        // 清空日志窗口
        QMetaObject::invokeMethod(pthis->log_window, "clear",
                      Qt::QueuedConnection);
    }
    QString q_message = QString::fromStdString(message);

    std::lock_guard<std::mutex> lock(pthis->log_mutex);
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    QString log_message = QString("[%1] %2").arg(current_date).arg(q_message);
    if (level != debug)
    {
        QMetaObject::invokeMethod(pthis->log_window, "appendPlainText",
                      Qt::QueuedConnection, Q_ARG(QString, log_message));
    }
#ifdef DEBUG
    std::cout << log_message.toStdString() << std::endl;
#endif
}

void init_logger(QPlainTextEdit* log_window)
{
    if (Logger::pthis == nullptr)
    {
        Logger::pthis = new Logger(log_window);
    }
}

