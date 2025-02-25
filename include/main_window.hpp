//
// Created by JellyfishKnight on 2025/2/25.
//

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "ui_main_window.h"

class PaperTrackMainWindow: public QWidget {
public:
    PaperTrackMainWindow(QWidget *parent = nullptr) :
        QWidget(parent) {
        ui.setupUi(this);
        bound_pages();
    }

private:
    void bound_pages();


    Ui_PaperTrackMainWindow ui;
};


#endif //MAIN_WINDOW_HPP
