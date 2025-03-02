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
#include "image_downloader.hpp"

class PaperTrackMainWindow : public QWidget {
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr);
    ~PaperTrackMainWindow() override;

private slots:
    void onSendButtonClicked();
    void onBrightnessChanged(int value);
    void onUseUserCameraClicked(int value);

    void onSSIDTextFocusIn();
    void onSSIDTextFocusOut();
    void onPasswordTextFocusIn();
    void onPasswordTextFocusOut();
private:
    std::string current_ip_;
    void bound_pages();
    void flashESP32();
    std::string getPortFromSerialManager(); // 获取串口端口名
    // UI组件
    Ui::PaperTrackerMainWindow ui{};

    cv::Rect roi_rect;
    bool is_roi_end = true;

    VideoReader video_reader;
    std::thread show_video_thread;

    // 推理和OSC通信
    Inference inference;
    OscManager osc_manager_;

    // 串口通信
    SerialPortManager serial_port_manager_;

    // 线程控制
    std::atomic<bool> window_closed{false};
    // 图像下载器
    ESP32VideoStream image_downloader_;
    std::queue<cv::Mat> image_buffer_queue;

    bool use_user_camera = false;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};