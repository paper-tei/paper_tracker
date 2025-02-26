//
// Created by JellyfishKnight on 2025/2/25.
//

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "ui_main_window.h"
#include "serial.hpp"

class PaperTrackMainWindow final : public QWidget {
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr) :
        QWidget(parent) {
        ui.setupUi(this);
        bound_pages();
    }



private:
    void bound_pages();

    SerialPortManager serial_port_manager_;

    Ui_PaperTrackMainWindow ui{};
};


#endif //MAIN_WINDOW_HPP
