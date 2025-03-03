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
#include "wifi_cache_file_writer.hpp"

class PaperTrackMainWindow : public QWidget {
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr);
    ~PaperTrackMainWindow() override;

private slots:
    void sendBrightnessValue();
    void onSendButtonClicked();
    void onBrightnessChanged(int value);
    void onUseUserCameraClicked(int value);
    void onRestartButtonClicked();
    void onSSIDTextFocusIn();
    void onSSIDTextFocusOut();
    void onPasswordTextFocusIn();
    void onPasswordTextFocusOut();
private:
    QTimer* brightness_timer;
    int current_brightness;
    std::string current_ip_;
    void bound_pages();
    void flashESP32();
    void restartESP32();
    std::string getPortFromSerialManager(); // 获取串口端口名
    // 根据模型输出更新校准页面的进度条
    void updateCalibrationProgressBars(const std::vector<float>& output);

    // 保存ARKit模型输出的映射表
    std::map<std::string, size_t> blendShapeIndexMap;

    // 初始化ARKit模型输出的映射表
    void initBlendShapeIndexMap();
    void start_image_download();

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

    // WiFi配置文件写入器
    WifiCacheFileWriter wifi_cache_file_writer;

    bool use_user_camera = false;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};