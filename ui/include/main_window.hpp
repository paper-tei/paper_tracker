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

class PaperTrackMainWindow : public QWidget {
private:
    // UI组件
    Ui::PaperTrackerMainWindow ui{};
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr);
    ~PaperTrackMainWindow() override;

    using FuncWithoutArgs = std::function<void()>;
    using FuncWithVal = std::function<void(int)>;
    // let user decide what to do with these action
    void setSendBrightnessValueFunc(FuncWithVal func);
    void setOnSendButtonClickedFunc(FuncWithoutArgs func);
    void setOnUseFilterClickedFunc(FuncWithVal func);
    void setOnRestartButtonClickedFunc(FuncWithoutArgs func);
    void setOnFlashButtonClickedFunc(FuncWithoutArgs func);

    void setSerialStatusLabel(const std::string& text) const { ui.SerialConnectLabel->setText(QString::fromStdString(text)); }
    void setWifiStatusLabel(const std::string& text) const { ui.WifiConnectLabel->setText(QString::fromStdString(text)); }
    void setIPText(const std::string& text) const { ui.textEdit->setText(QString::fromStdString(text)); }

    [[no_discard]] QPlainTextEdit* getLogText() const { return ui.LogText; }
    [[no_discard]] std::tuple<cv::Rect, bool> getRoiRect() const { return std::make_tuple(roi_rect, is_roi_end); }

    [[no_discard]] float getRotateAngle() const;
    [[no_discard]] std::string getSSID() const { return ui.SSIDText->toPlainText().toStdString(); }
    [[no_discard]] std::string getPassword() const { return ui.PasswordText->toPlainText().toStdString(); }

    void setVideoImage(const cv::Mat& image);

    // 根据模型输出更新校准页面的进度条
    void updateCalibrationProgressBars(
        const std::vector<float>& output,
        const std::unordered_map<std::string, size_t>& blendShapeIndexMap
    );

private slots:
    void onBrightnessChanged(int value);
    void onRotateAngleChanged(int value);

private:
    QProcess* vrcftProcess;

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

    cv::Rect roi_rect;
    bool is_roi_end = true;

    // 线程控制
    std::atomic<bool> window_closed{false};
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};