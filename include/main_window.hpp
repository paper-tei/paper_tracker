//
// Created by JellyfishKnight on 2025/2/25.
//

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <future>
#include <inference.hpp>
#include <thread>

#include "osc.hpp"
#include "ui_main_window.h"
#include "video_reader.hpp"
#include "serial.hpp"
#include "roi_event.hpp"

class PaperTrackMainWindow final : public QWidget {
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr);

    ~PaperTrackMainWindow();

private:
    bool window_closed = false;

    std::thread show_video_thread;

    void bound_pages();

    VideoReader video_reader;
    Inference inference;
    SerialPortManager serial_port_manager_;

    cv::Rect roi_rect;

    // OSC设置相关变量
    OscManager osc_manager_;
    std::string osc_address_ = "127.0.0.1";
    int osc_port_ = 8888;
    std::string osc_prefix_ = "";
    float osc_multiplier_ = 1.0f;
    bool osc_enabled_ = true;

    Ui_PaperTrackerMainWindow ui{};
};


#endif //MAIN_WINDOW_HPP
