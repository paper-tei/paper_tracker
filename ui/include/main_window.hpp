//
// Created by JellyfishKnight on 2025/2/25.
//

#pragma once
#include <QProcess>
#include <thread>
#include <future>
#include <atomic>
#include <algorithm>
#include <QTimer>

#include "ui_main_window.h"
#include "inference.hpp"
#include "serial.hpp"
#include "wifi_cache_file_writer.hpp"
#include "logger.hpp"

struct Rect
{
    cv::Rect rect;
    bool is_roi_end = true;
};

class PaperTrackMainWindow : public QWidget {
private:
    // UI组件
    Ui::PaperTrackerMainWindow ui{};
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr);
    ~PaperTrackMainWindow() override;

    void setSerialStatusLabel(const std::string& text) const;
    void setWifiStatusLabel(const std::string& text) const;
    void setIPText(const std::string& text) const;

    QPlainTextEdit* getLogText() const;
    Rect getRoiRect() const;
    float getRotateAngle() const;
    std::string getSSID() const;
    std::string getPassword() const;

    void setVideoImage(const cv::Mat& image);
    // 根据模型输出更新校准页面的进度条
    void updateCalibrationProgressBars(
        const std::vector<float>& output,
        const std::unordered_map<std::string, size_t>& blendShapeIndexMap
    );

    using FuncWithoutArgs = std::function<void()>;
    using FuncWithVal = std::function<void(int)>;
    // let user decide what to do with these action
    void setSendBrightnessValueFunc(FuncWithVal func);
    void setOnSendButtonClickedFunc(FuncWithoutArgs func);
    void setOnUseFilterClickedFunc(FuncWithVal func);
    void setOnRestartButtonClickedFunc(FuncWithoutArgs func);
    void setOnFlashButtonClickedFunc(FuncWithoutArgs func);

    void setBeforeStop(FuncWithoutArgs func);

    void set_update_thread(FuncWithoutArgs func);
    void set_inference_thread(FuncWithoutArgs func);

    bool is_running() const;

    void stop();
    int get_max_fps() const;

private slots:
    void onBrightnessChanged(int value);
    void onSendBrightnessValue() const;
    void onRotateAngleChanged(int value);
    void onSendButtonClicked() const;
    void onRestartButtonClicked();
    void onUseFilterClicked(int value) const;
    void onFlashButtonClicked();

    void onEnergyModeChanged(int value);
private:
    QProcess* vrcftProcess;

    FuncWithoutArgs beforeStopFunc;

    FuncWithoutArgs onSendButtonClickedFunc;
    FuncWithVal sendBrightnessValueFunc;
    FuncWithVal onRotateAngleChangedFunc;
    FuncWithoutArgs onRestartButtonClickedFunc;
    FuncWithVal onUseFilterClickedFunc;
    FuncWithoutArgs onFlashButtonClickedFunc;

    QTimer* brightness_timer;

    int current_brightness;
    int current_rotate_angle = 0;

    std::string current_ip_;
    void bound_pages();

    void connect_callbacks();

    Rect roi_rect;

    std::thread update_thread;
    std::thread inference_thread;
    bool app_is_running = true;
    int max_fps = 38;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};