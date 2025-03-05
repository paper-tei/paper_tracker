//
// Created by JellyfishKnight on 2025/2/25.
//
/*
 * PaperTracker - 面部追踪应用程序
 * Copyright (C) 2025 PAPER TRACKER
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file contains code from projectbabble:
 * Copyright 2023 Sameer Suri
 * Licensed under the Apache License, Version 2.0
 */
#include "main_window.hpp"
#include <QMessageBox>
#include <codecvt>
#include <locale>
#include <windows.h>
#include <QTimer>
#include <QProcess>
#include <QCoreApplication>
#include <roi_event.hpp>

PaperTrackMainWindow::PaperTrackMainWindow(QWidget *parent)
    : QWidget(parent)
{
    // 基本UI设置
    setFixedSize(848, 538);
    ui.setupUi(this);
    ui.LogText->setMaximumBlockCount(200);
    init_logger(ui.LogText);
    LOG_INFO("系统初始化中...");
    // 初始化串口连接状态
    ui.SerialConnectLabel->setText("串口未连接");
    ui.WifiConnectLabel->setText("Wifi未连接");

    // 初始化亮度控制相关成员
    current_brightness = 0;
    // 连接信号和槽
    connect_callbacks();
    // 添加输入框焦点事件处理
    ui.SSIDText->installEventFilter(this);
    ui.PasswordText->installEventFilter(this);
    // 允许Tab键在输入框之间跳转
    ui.SSIDText->setTabChangesFocus(true);
    ui.PasswordText->setTabChangesFocus(true);
    // 清除所有控件的初始焦点，确保没有文本框自动获得焦点
    setFocus();

    // 添加ROI事件
    ROIEventFilter *roiFilter = new ROIEventFilter([this] (QRect rect, bool isEnd)
    {
        roi_rect.is_roi_end = isEnd;
        roi_rect = Rect(cv::Rect(rect.x(), rect.y(), rect.width(), rect.height()));
        // if end point is out of image, move rect
        if (roi_rect.rect.x < 0)
        {
            roi_rect.rect.width += rect.x();
            roi_rect.rect.x = 0;
        }
        if (roi_rect.rect.y < 0)
        {
            roi_rect.rect.height += rect.y();
            roi_rect.rect.y = 0;
        }
        // if roi is bigger than image, resize rect
        if (roi_rect.rect.x + roi_rect.rect.width > 280)
        {
            roi_rect.rect.width = 280 - roi_rect.rect.x;
        }
        if (roi_rect.rect.y + roi_rect.rect.height > 280)
        {
            roi_rect.rect.height = 280 - roi_rect.rect.y;
        }
    },ui.ImageLabel);
    ui.ImageLabel->installEventFilter(roiFilter);
    ui.ImageLabelCal->installEventFilter(roiFilter);

    // 初始化页面导航
    bound_pages();

    LOG_INFO("系统初始化完成, 正在启动VRCFT");

    vrcftProcess = new QProcess(this);
    // 启动VRCFT应用程序
    // 尝试方法2: PowerShell查找并启动
    QString command2 = "powershell -Command \"$pkg = Get-AppxPackage | Where-Object {$_.Name -like '*96ba052f*'}; if($pkg) { Start-Process ($pkg.InstallLocation + '\\VRCFaceTracking.exe') }\"";
    vrcftProcess->start(command2);
    // 连接信号以检测进程状态
    connect(vrcftProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit) {
            LOG_INFO("VRCFT已正常退出，退出码: " + std::to_string(exitCode));
        } else {
            LOG_INFO("VRCFT异常退出");
        }
    });
}

void PaperTrackMainWindow::setVideoImage(const cv::Mat& image)
{
    auto qimage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    // 使用Qt的线程安全方式更新UI
    QMetaObject::invokeMethod(this, [this, qimage]() {
        auto pix_map = QPixmap::fromImage(qimage);
        if (ui.stackedWidget->currentIndex() == 0)
        {
            ui.ImageLabel->setPixmap(pix_map);
            ui.ImageLabel->setScaledContents(true);
            ui.ImageLabel->update();
        } else if (ui.stackedWidget->currentIndex() == 1) {
            ui.ImageLabelCal->setPixmap(pix_map);
            ui.ImageLabelCal->setScaledContents(true);
            ui.ImageLabelCal->update();
        }
    }, Qt::QueuedConnection);
}

PaperTrackMainWindow::~PaperTrackMainWindow() = default;

void PaperTrackMainWindow::bound_pages() {
    // 页面导航逻辑
    connect(ui.MainPageButton, &QPushButton::clicked, [this] {
        ui.stackedWidget->setCurrentIndex(0);
    });
    connect(ui.CalibrationPageButton, &QPushButton::clicked, [this] {
        ui.stackedWidget->setCurrentIndex(1);
    });
}

// 添加事件过滤器实现
bool PaperTrackMainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // 处理焦点获取事件
    if (event->type() == QEvent::FocusIn) {
        if (obj == ui.SSIDText) {
            if (ui.SSIDText->toPlainText() == "请输入WIFI名字（仅支持2.4ghz）") {
                ui.SSIDText->setPlainText("");
            }
        } else if (obj == ui.PasswordText) {
            if (ui.PasswordText->toPlainText() == "请输入WIFI密码") {
                ui.PasswordText->setPlainText("");
            }
        }
    }

    // 处理焦点失去事件
    if (event->type() == QEvent::FocusOut) {
        if (obj == ui.SSIDText) {
            if (ui.SSIDText->toPlainText().isEmpty()) {
                ui.SSIDText->setPlainText("请输入WIFI名字（仅支持2.4ghz）");
            }
        } else if (obj == ui.PasswordText) {
            if (ui.PasswordText->toPlainText().isEmpty()) {
                ui.PasswordText->setPlainText("请输入WIFI密码");
            }
        }
    }

    // 继续事件处理
    return QWidget::eventFilter(obj, event);
}

// 根据模型输出更新校准页面的进度条
void PaperTrackMainWindow::updateCalibrationProgressBars(
    const std::vector<float>& output,
    const std::unordered_map<std::string, size_t>& blendShapeIndexMap
) {
    if (output.empty() || ui.stackedWidget->currentIndex() != 1) {
        // 如果输出为空或者当前不在校准页面，则不更新
        return;
    }

    // 使用Qt的线程安全方式更新UI
    QMetaObject::invokeMethod(this, [this, output, &blendShapeIndexMap]() {
        // 将值缩放到0-100范围内用于进度条显示
        auto scaleValue = [](const float value) -> int {
            // 将值限制在0-1.0范围内，然后映射到0-100
            return static_cast<int>(value * 100);
        };

        // 更新各个进度条
        // 注意：这里假设输出数组中的索引与ARKit模型输出的顺序一致

        // 脸颊
        if (blendShapeIndexMap.contains("cheekPuffLeft") && blendShapeIndexMap.at("cheekPuffLeft") < output.size()) {
            ui.CheekPullLeftValue->setValue(scaleValue(output[blendShapeIndexMap.at("cheekPuffLeft")]));
        }
        if (blendShapeIndexMap.contains("cheekPuffRight") && blendShapeIndexMap.at("cheekPuffRight") < output.size()) {
            ui.CheekPullRightValue->setValue(scaleValue(output[blendShapeIndexMap.at("cheekPuffRight")]));
        }
        // 下巴
        if (blendShapeIndexMap.contains("jawOpen") && blendShapeIndexMap.at("jawOpen") < output.size()) {
            ui.JawOpenValue->setValue(scaleValue(output[blendShapeIndexMap.at("jawOpen")]));
        }
        if (blendShapeIndexMap.contains("jawLeft") && blendShapeIndexMap.at("jawLeft") < output.size()) {
            ui.JawLeftValue->setValue(scaleValue(output[blendShapeIndexMap.at("jawLeft")]));
        }
        if (blendShapeIndexMap.contains("jawRight") && blendShapeIndexMap.at("jawRight") < output.size()) {
            ui.JawRightValue->setValue(scaleValue(output[blendShapeIndexMap.at("jawRight")]));
        }
        // 嘴巴
        if (blendShapeIndexMap.contains("mouthLeft") && blendShapeIndexMap.at("mouthLeft") < output.size()) {
            ui.MouthLeftValue->setValue(scaleValue(output[blendShapeIndexMap.at("mouthLeft")]));
        }
        if (blendShapeIndexMap.contains("mouthRight") && blendShapeIndexMap.at("mouthRight") < output.size()) {
            ui.MouthRightValue->setValue(scaleValue(output[blendShapeIndexMap.at("mouthRight")]));
        }
        // 舌头
        if (blendShapeIndexMap.contains("tongueOut") && blendShapeIndexMap.at("tongueOut") < output.size()) {
            ui.TongueOutValue->setValue(scaleValue(output[blendShapeIndexMap.at("tongueOut")]));
        }
        if (blendShapeIndexMap.contains("tongueUp") && blendShapeIndexMap.at("tongueUp") < output.size()) {
            ui.TongueUpValue->setValue(scaleValue(output[blendShapeIndexMap.at("tongueUp")]));
        }
        if (blendShapeIndexMap.contains("tongueDown") && blendShapeIndexMap.at("tongueDown") < output.size()) {
            ui.TongueDownValue->setValue(scaleValue(output[blendShapeIndexMap.at("tongueDown")]));
        }
        if (blendShapeIndexMap.contains("tongueLeft") && blendShapeIndexMap.at("tongueLeft") < output.size()) {
            ui.TongueLeftValue->setValue(scaleValue(output[blendShapeIndexMap.at("tongueLeft")]));
        }
        if (blendShapeIndexMap.contains("tongueRight") && blendShapeIndexMap.at("tongueRight") < output.size()) {
            ui.TongueRightValue->setValue(scaleValue(output[blendShapeIndexMap.at("tongueRight")]));
        }
    }, Qt::QueuedConnection);
}

void PaperTrackMainWindow::connect_callbacks()
{
    brightness_timer = new QTimer(this);
    brightness_timer->setSingleShot(true);
    connect(brightness_timer, &QTimer::timeout, this, &PaperTrackMainWindow::onSendBrightnessValue);
    // functions
    connect(ui.BrightnessBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onBrightnessChanged);
    connect(ui.RotateImageBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onRotateAngleChanged);
    connect(ui.restart_Button, &QPushButton::clicked, this, &PaperTrackMainWindow::onRestartButtonClicked);
    connect(ui.FlashFirmwareButton, &QPushButton::clicked, this, &PaperTrackMainWindow::onFlashButtonClicked);
    connect(ui.UseFilterBox, &QCheckBox::checkStateChanged, this, &PaperTrackMainWindow::onUseFilterClicked);
    connect(ui.wifi_send_Button, &QPushButton::clicked, this, &PaperTrackMainWindow::onSendButtonClicked);
    connect(ui.EnergyModeBox, &QComboBox::currentIndexChanged, this, &PaperTrackMainWindow::onEnergyModeChanged);
}

float PaperTrackMainWindow::getRotateAngle() const
{
    auto rotate_angle = static_cast<float>(current_rotate_angle);
    rotate_angle = rotate_angle / (static_cast<float>(ui.RotateImageBar->maximum()) -
        static_cast<float>(ui.RotateImageBar->minimum())) * 360.0f;
    return rotate_angle;
}

void PaperTrackMainWindow::setSendBrightnessValueFunc(FuncWithVal func)
{
    sendBrightnessValueFunc = std::move(func);
    // need to bound with timer after setting the function
}

void PaperTrackMainWindow::setOnFlashButtonClickedFunc(FuncWithoutArgs func)
{
    onFlashButtonClickedFunc = std::move(func);
}

void PaperTrackMainWindow::setOnUseFilterClickedFunc(FuncWithVal func)
{
    onUseFilterClickedFunc = std::move(func);
}

void PaperTrackMainWindow::setOnSendButtonClickedFunc(FuncWithoutArgs func)
{
    onSendButtonClickedFunc = std::move(func);
}

void PaperTrackMainWindow::setOnRestartButtonClickedFunc(FuncWithoutArgs func)
{
    onRestartButtonClickedFunc = std::move(func);
}

void PaperTrackMainWindow::setSerialStatusLabel(const std::string& text) const
{
    ui.SerialConnectLabel->setText(QString::fromStdString(text));
}

void PaperTrackMainWindow::setWifiStatusLabel(const std::string& text) const
{
    ui.WifiConnectLabel->setText(QString::fromStdString(text));
}

void PaperTrackMainWindow::setIPText(const std::string& text) const
{
    ui.textEdit->setText(QString::fromStdString(text));
}

QPlainTextEdit* PaperTrackMainWindow::getLogText() const
{
    return ui.LogText;
}

Rect PaperTrackMainWindow::getRoiRect() const
{
    return roi_rect;
}

std::string PaperTrackMainWindow::getSSID() const
{
    return ui.SSIDText->toPlainText().toStdString();
}

std::string PaperTrackMainWindow::getPassword() const
{
    return ui.PasswordText->toPlainText().toStdString();
}

void PaperTrackMainWindow::onSendButtonClicked() const
{
    onSendButtonClickedFunc();
    // need to restart esp32 after sending Wi-Fi config
    onRestartButtonClickedFunc();
}

void PaperTrackMainWindow::onRestartButtonClicked()
{
    onRestartButtonClickedFunc();
}

void PaperTrackMainWindow::onUseFilterClicked(int value) const
{
    onUseFilterClickedFunc(value);
}

void PaperTrackMainWindow::onFlashButtonClicked()
{
    onFlashButtonClickedFunc();
}

void PaperTrackMainWindow::onBrightnessChanged(int value) {
    // 更新当前亮度值
    current_brightness = value;
    // 重置定时器，如果用户500毫秒内没有再次改变值，就发送数据包
    brightness_timer->start(100);
}

void PaperTrackMainWindow::onRotateAngleChanged(int value)
{
    current_rotate_angle = value;
}

void PaperTrackMainWindow::onSendBrightnessValue() const
{
    sendBrightnessValueFunc(current_brightness);
}

void PaperTrackMainWindow::setBeforeStop(FuncWithoutArgs func)
{
    beforeStopFunc = std::move(func);
}

void PaperTrackMainWindow::set_update_thread(FuncWithoutArgs func)
{
    update_thread = std::thread(std::move(func));
}

void PaperTrackMainWindow::set_inference_thread(FuncWithoutArgs func)
{
    inference_thread = std::thread(std::move(func));
}

bool PaperTrackMainWindow::is_running() const
{
    return app_is_running;
}

void PaperTrackMainWindow::stop()
{
    LOG_INFO("正在关闭系统...");
    app_is_running = false;
    if (update_thread.joinable())
    {
        update_thread.join();
    }
    if (inference_thread.joinable())
    {
        inference_thread.join();
    }
    if (brightness_timer) {
        brightness_timer->stop();
        delete brightness_timer;
    }
    if (beforeStopFunc)
    {
        beforeStopFunc();
    }
    if (vrcftProcess) {
        if (vrcftProcess->state() == QProcess::Running) {
            vrcftProcess->terminate();
            if (!vrcftProcess->waitForFinished(3000)) {  // 等待最多3秒
                vrcftProcess->kill();  // 如果进程没有及时终止，则强制结束
            }
        }
        delete vrcftProcess;
    }
    // 其他清理工作
    LOG_INFO("系统已安全关闭");
}

void PaperTrackMainWindow::onEnergyModeChanged(int index)
{
    if (index == 0)
    {
        max_fps = 38;
    } else if (index == 1)
    {
        max_fps = 15;
    } else if (index == 2)
    {
        max_fps = 70;
    }
}

int PaperTrackMainWindow::get_max_fps() const
{
    return max_fps;
}


