//
// Created by JellyfishKnight on 2025/2/25.
//

#pragma once
#include <QProcess>
#include <QWidget>
#include <thread>
#include <future>
#include <atomic>
#include <algorithm>
#include <QTimer>

#include "ui_main_window.h"
#include "video_reader.hpp"
#include "inference.hpp"
#include "osc.hpp"
#include "serial.hpp"
#include "roi_event.hpp"
#include "image_downloader.hpp"
#include "wifi_cache_file_writer.hpp"
#include "logger.hpp"

class PaperTrackMainWindow : public QWidget {
private:
    // UI组件
    Ui::PaperTrackerMainWindow ui{};
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr);
    ~PaperTrackMainWindow() override;
    QProcess* vrcftProcess;
private slots:
    void sendBrightnessValue();

    void onSendButtonClicked();
    void onBrightnessChanged(int value);
    void onRotateAngleChanged(int value);
    void onUseUserCameraClicked(int value);
    void onRestartButtonClicked();
    void onUseFilterClicked(int value);
public:
    using FuncWithoutArgs = std::function<void()>;
    using FuncWithVal = std::function<void(int)>;
    // let user decide what to do with these action
    void setSendBrightnessValueFunc(FuncWithVal func) { sendBrightnessValueFunc = std::move(func); }
    void setOnSendButtonClickedFunc(FuncWithoutArgs func) { onSendButtonClickedFunc = std::move(func); }
    void setOnUseFilterClickedFunc(FuncWithVal func) { onUseFilterClickedFunc = std::move(func); }
    void setOnUseUserCameraClickedFunc(FuncWithVal func) { onUseUserCameraClickedFunc = std::move(func); }
    void setOnRestartButtonClickedFunc(FuncWithoutArgs func) { onRestartButtonClickedFunc = std::move(func); }
    void setOnRotateAngleChangedFunc(FuncWithVal func) { onRotateAngleChangedFunc = std::move(func); }

private:
    FuncWithoutArgs onSendButtonClickedFunc;
    FuncWithVal sendBrightnessValueFunc;
    FuncWithVal onRotateAngleChangedFunc;
    FuncWithVal onUseUserCameraClickedFunc;
    FuncWithoutArgs onRestartButtonClickedFunc;
    FuncWithVal onUseFilterClickedFunc;

    QTimer* brightness_timer;

    int current_brightness;
    int current_rotate_angle = 0;

    std::string current_ip_;
    void bound_pages();

    void connect_callbacks();

    void flashESP32();
    void restartESP32();
    std::string getPortFromSerialManager(); // 获取串口端口名
    // 根据模型输出更新校准页面的进度条
    void updateCalibrationProgressBars(const std::vector<float>& output);
    void AmpMapToOutput(std::vector<float>& output);

    // 保存ARKit模型输出的映射表
    std::unordered_map<std::string, size_t> blendShapeIndexMap;
    std::unordered_map<std::string, int> blendShapeAmpMap;
    std::vector<std::string> blendShapes;
    // 初始化ARKit模型输出的映射表
    void initBlendShapeIndexMap();

    void start_image_download();


    cv::Rect roi_rect;
    bool is_roi_end = true;

    std::shared_ptr<VideoReader> video_reader;
    std::thread show_video_thread;

    // 推理和OSC通信
    std::shared_ptr<Inference> inference;
    std::shared_ptr<OscManager> osc_manager_;
    // 串口通信
    std::shared_ptr<SerialPortManager> serial_port_manager_;

    // 线程控制
    std::atomic<bool> window_closed{false};
    // 图像下载器
    std::shared_ptr<ESP32VideoStream> image_downloader_;
    std::queue<cv::Mat> image_buffer_queue;

    // WiFi配置文件写入器
    std::shared_ptr<WifiCacheFileWriter> wifi_cache_file_writer;

    bool use_user_camera = false;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};