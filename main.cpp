#include <QApplication>
#include <QPushButton>
#include <QFile>

#include "main_window.hpp"
#include <curl/curl.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    curl_easy_init();

    QFile qssFile("./resources/material.qss"); // 使用资源路径
    if (qssFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(qssFile.readAll());
        app.setStyleSheet(styleSheet);
        qssFile.close();
    } else {
        qWarning("无法打开 QSS 文件");
    }

    PaperTrackMainWindow window;
    window.setWindowIcon(QIcon("./resources/window_icon.png"));  // 设置窗口图标
    window.show();

    return QApplication::exec();
}
