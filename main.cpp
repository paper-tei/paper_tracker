#include <QApplication>
#include <QPushButton>

#include "main_window.hpp"

#include <curl/curl.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    curl_easy_init();

    PaperTrackMainWindow window;
    window.setWindowIcon(QIcon("./resources/window_icon.png"));  // 设置窗口图标
    window.show();

    return QApplication::exec();
}
