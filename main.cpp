#include <QApplication>
#include <QPushButton>

#include "logger.h"
#include "main_window.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    PaperTrackMainWindow window;
    window.show();

    return QApplication::exec();
}
