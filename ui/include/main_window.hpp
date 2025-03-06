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
#include "logger.hpp"
#include "reflect.hpp"

struct Rect
{
    Rect() = default;

    Rect(int x, int y, int width, int height) :
        rect(x, y, width, height), x(x), y(y), width(width), height(height) {}

    explicit Rect(cv::Rect rect) : rect(rect), x(rect.x), y(rect.y), width(rect.width), height(rect.height) {}

    cv::Rect rect;
    bool is_roi_end = true;

    int x{};
    int y{};
    int width{};
    int height{};

    REFLECT(x, y, width, height);
};

struct PaperTrackerConfig
{
    int brightness;
    int rotate_angle;
    int energy_mode;
    std::string wifi_ip;
    bool use_filter;
    std::unordered_map<std::string, int> amp_map;
    Rect rect;

    REFLECT(brightness, rotate_angle, energy_mode, wifi_ip, use_filter, amp_map, rect);
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
    void setOnSaveConfigButtonClickedFunc(FuncWithoutArgs func);
    void setOnAmpMapChangedFunc(FuncWithoutArgs func);


    void setBeforeStop(FuncWithoutArgs func);

    void set_update_thread(FuncWithoutArgs func);
    void set_inference_thread(FuncWithoutArgs func);

    bool is_running() const;

    void stop();
    int get_max_fps() const;

    PaperTrackerConfig generate_config() const;

    void set_config(const PaperTrackerConfig& config);

    std::unordered_map<std::string, int> getAmpMap() const;

private slots:
    void onBrightnessChanged(int value);
    void onSendBrightnessValue() const;
    void onRotateAngleChanged(int value);
    void onSendButtonClicked() const;
    void onRestartButtonClicked();
    void onUseFilterClicked(int value) const;
    void onFlashButtonClicked();
    void onEnergyModeChanged(int value);
    void onSaveConfigButtonClicked() const;

    void onCheeckPuffLeftChanged(int value) const;
    void onCheeckPuffRightChanged(int value);
    void onJawOpenChanged(int value);
    void onJawLeftChanged(int value);
    void onJawRightChanged(int value);
    void onMouthLeftChanged(int value);
    void onMouthRightChanged(int value);
    void onTongueOutChanged(int value);
    void onTongueLeftChanged(int value);
    void onTongueRightChanged(int value);
    void onTongueUpChanged(int value);
    void onTongueDownChanged(int value);
private:
    QProcess* vrcftProcess;

    FuncWithoutArgs beforeStopFunc;

    FuncWithoutArgs onSendButtonClickedFunc;
    FuncWithVal sendBrightnessValueFunc;
    FuncWithVal onRotateAngleChangedFunc;
    FuncWithoutArgs onRestartButtonClickedFunc;
    FuncWithVal onUseFilterClickedFunc;
    FuncWithoutArgs onFlashButtonClickedFunc;
    FuncWithoutArgs onSaveConfigButtonClickedFunc;
    FuncWithoutArgs onAmpMapChangedFunc;

    QTimer* brightness_timer{};

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