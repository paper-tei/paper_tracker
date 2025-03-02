#include <QApplication>
#include <QPushButton>

#include "logger.h"
#include "main_window.hpp"

#include <curl/curl.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    curl_easy_init();

    PaperTrackMainWindow window;
    window.show();

    return QApplication::exec();
}
