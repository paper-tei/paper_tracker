//
// Created by JellyfishKnight on 2025/2/25.
//

#pragma once

#include <QWidget>
#include <thread>
#include <future>
#include <atomic>

#include "ui_main_window.h"
#include "video_reader.hpp"
#include "inference.hpp"
#include "osc.hpp"
#include "serial.hpp"
#include "roi_event.hpp"

class PaperTrackMainWindow : public QWidget {
   // Q_OBJECT // 这行非常重要，不能缺少

public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr);
    ~PaperTrackMainWindow() override;

    private slots:
    void onSendButtonClicked();
    void onBrightnessChanged(int value);

private:
    void bound_pages();

    // UI组件
    Ui::PaperTrackerMainWindow ui{};

    cv::Rect roi_rect;
    VideoReader video_reader;
    std::thread show_video_thread;

    // 推理和OSC通信
    Inference inference;
    OscManager osc_manager_;

    // 串口通信
    SerialPortManager serial_port_manager_;

    // 线程控制
    std::atomic<bool> window_closed{false};
};