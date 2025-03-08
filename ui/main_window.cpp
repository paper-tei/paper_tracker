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
    // 初始化页面导航
    bound_pages();

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
    auto *roiFilter = new ROIEventFilter([this] (QRect rect, bool isEnd)
    {
        roi_rect.is_roi_end = isEnd;
        roi_rect = Rect(rect.x(), rect.y(), rect.width(), rect.height());
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

    // 初始化串口和wifi
    serial_port_manager = std::make_shared<SerialPortManager>();
    image_downloader = std::make_shared<ESP32VideoStream>();
    LOG_INFO("初始化串口");
    serial_port_manager->init();
    // init serial port manager
    serial_port_manager->setDeviceStatusCallback([this]
                                                        (const std::string& ip, int brightness, int power, int version) {
        // 使用Qt的线程安全方式更新UI
        QMetaObject::invokeMethod(this, [ip, brightness, power, version, this]() {
            // 只在 IP 地址变化时更新显示
            if (current_ip_ != ip)
            {
                current_ip_ = "http://" + ip;
                // 更新IP地址显示，添加 http:// 前缀
                this->setIPText(current_ip_);
                LOG_INFO("IP地址已更新: " + current_ip_);


                start_image_download();
            }
            // 可以添加其他状态更新的日志，如果需要的话
        }, Qt::QueuedConnection);
    });

    LOG_INFO("等待串口状态响应");
    while (serial_port_manager->status() == SerialStatus::CLOSED) {}
    LOG_INFO("串口状态响应完毕");

    if (serial_port_manager->status() == SerialStatus::FAILED)
    {
        setSerialStatusLabel("串口连接失败");
        LOG_WARN("串口未连接，尝试从配置文件中读取地址...");
        if (!config.wifi_ip.empty())
        {
            LOG_INFO("从配置文件中读取地址成功");
            current_ip_ = config.wifi_ip;
            set_config(config);
            start_image_download();
        } else
        {
            QMessageBox msgBox;
            msgBox.setWindowIcon(this->windowIcon());
            msgBox.setText("未找到配置文件信息，请将面捕通过数据线连接到电脑进行首次配置");
            msgBox.exec();
        }
    } else
    {
        LOG_INFO("串口连接成功");
        setSerialStatusLabel("串口连接成功");
    }

    // restart_check_timer->start(1000);
}

void PaperTrackMainWindow::setVideoImage(const cv::Mat& image)
{
    if (image.empty())
    {
        QMetaObject::invokeMethod(this, [this]() {
            if (ui.stackedWidget->currentIndex() == 0) {
                ui.ImageLabel->clear(); // 清除图片
                ui.ImageLabel->setText("                         没有图像输入"); // 恢复默认文本
            } else if (ui.stackedWidget->currentIndex() == 1) {
                ui.ImageLabelCal->clear(); // 清除图片
                ui.ImageLabelCal->setText("                         没有图像输入");
            }
        }, Qt::QueuedConnection);
        return ;
    }
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
    brightness_timer = std::make_shared<QTimer>();
    brightness_timer->setSingleShot(true);
    restart_check_timer = std::make_shared<QTimer>();
    connect(restart_check_timer.get(), &QTimer::timeout, this, &PaperTrackMainWindow::restartCheck);
    connect(brightness_timer.get(), &QTimer::timeout, this, &PaperTrackMainWindow::onSendBrightnessValue);
    // functions
    connect(ui.BrightnessBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onBrightnessChanged);
    connect(ui.RotateImageBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onRotateAngleChanged);
    connect(ui.restart_Button, &QPushButton::clicked, this, &PaperTrackMainWindow::onRestartButtonClicked);
    connect(ui.FlashFirmwareButton, &QPushButton::clicked, this, &PaperTrackMainWindow::onFlashButtonClicked);
    connect(ui.UseFilterBox, &QCheckBox::checkStateChanged, this, &PaperTrackMainWindow::onUseFilterClicked);
    connect(ui.wifi_send_Button, &QPushButton::clicked, this, &PaperTrackMainWindow::onSendButtonClicked);
    connect(ui.EnergyModeBox, &QComboBox::currentIndexChanged, this, &PaperTrackMainWindow::onEnergyModeChanged);
    connect(ui.SaveParamConfigButton, &QPushButton::clicked, this, &PaperTrackMainWindow::onSaveConfigButtonClicked);

    connect(ui.JawOpenBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onJawOpenChanged);
    connect(ui.JawLeftBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onJawLeftChanged);
    connect(ui.JawRightBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onJawRightChanged);
    connect(ui.MouthLeftBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onMouthLeftChanged);
    connect(ui.MouthRightBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onMouthRightChanged);
    connect(ui.TongueOutBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onTongueOutChanged);
    connect(ui.TongueLeftBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onTongueLeftChanged);
    connect(ui.TongueRightBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onTongueRightChanged);
    connect(ui.TongueUpBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onTongueUpChanged);
    connect(ui.TongueDownBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onTongueDownChanged);
    connect(ui.CheekPuffLeftBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onCheeckPuffLeftChanged);
    connect(ui.CheekPuffRightBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onCheeckPuffRightChanged);
}

float PaperTrackMainWindow::getRotateAngle() const
{
    auto rotate_angle = static_cast<float>(current_rotate_angle);
    rotate_angle = rotate_angle / (static_cast<float>(ui.RotateImageBar->maximum()) -
        static_cast<float>(ui.RotateImageBar->minimum())) * 360.0f;
    return rotate_angle;
}


void PaperTrackMainWindow::setOnUseFilterClickedFunc(FuncWithVal func)
{
    onUseFilterClickedFunc = std::move(func);
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

void PaperTrackMainWindow::onSendButtonClicked()
{
    // onSendButtonClickedFunc();
    // 获取SSID和密码
    auto ssid = getSSID();
    auto password = getPassword();

    // 输入验证
    if (ssid == "请输入WIFI名字（仅支持2.4ghz）" || ssid.empty()) {
        QMessageBox::warning(this, "输入错误", "请输入有效的WIFI名字");
        return;
    }

    if (password == "请输入WIFI密码" || password.empty()) {
        QMessageBox::warning(this, "输入错误", "请输入有效的密码");
        return;
    }

    // 构建并发送数据包
    LOG_INFO("已发送WiFi配置: SSID=" + ssid + ", PWD=" + password);
    LOG_INFO("等待数据被发送后开始自动重启ESP32...");
    serial_port_manager->sendWiFiConfig(ssid, password);

    QTimer::singleShot(4000, this, [this] {
        // 4秒后自动重启ESP32
        onRestartButtonClicked();
    });
}

void PaperTrackMainWindow::onRestartButtonClicked()
{
    serial_port_manager->restartESP32(this);
    image_downloader->stop();
    image_downloader->start();
}

void PaperTrackMainWindow::onUseFilterClicked(int value) const
{
    onUseFilterClickedFunc(value);
}

void PaperTrackMainWindow::onFlashButtonClicked()
{
    // onFlashButtonClickedFunc();
    serial_port_manager->flashESP32(this);
    image_downloader->stop();
    image_downloader->start();
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
    // 发送亮度控制命令 - 确保亮度值为三位数字
    std::string brightness_str = std::to_string(current_brightness);
    // 补齐三位数字，前面加0
    while (brightness_str.length() < 3) {
        brightness_str = std::string("0") + brightness_str;
    }
    std::string packet = "A6" + brightness_str + "B6";
    serial_port_manager->write_data(packet);
    // 记录操作
    LOG_INFO("已设置亮度: " + std::to_string(current_brightness));
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
    if (restart_check_timer)
    {
        restart_check_timer->stop();
        restart_check_timer.reset();
    }
    if (brightness_timer) {
        brightness_timer->stop();
        brightness_timer.reset();
    }
    serial_port_manager->stop();
    image_downloader->stop();
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

PaperTrackerConfig PaperTrackMainWindow::generate_config() const
{
    PaperTrackerConfig config;
    config.brightness = current_brightness;
    config.rotate_angle = current_rotate_angle;
    config.energy_mode = ui.EnergyModeBox->currentIndex();
    config.use_filter = ui.UseFilterBox->isChecked();
    config.wifi_ip = ui.textEdit->toPlainText().toStdString();
    config.amp_map = {
        {"cheekPuffLeft", ui.CheekPuffLeftBar->value()},
        {"cheekPuffRight", ui.CheekPuffRightBar->value()},
        {"jawOpen", ui.JawOpenBar->value()},
        {"jawLeft", ui.MouthLeftBar->value()},
        {"jawRight", ui.MouthRightBar->value()},
        {"mouthLeft", ui.TongueOutBar->value()},
        {"mouthRight", ui.TongueUpBar->value()},
        {"tongueOut", ui.TongueDownBar->value()},
        {"tongueUp", ui.TongueLeftBar->value()},
        {"tongueDown", ui.TongueRightBar->value()},
        {"tongueLeft", ui.JawLeftBar->value()},
        {"tongueRight", ui.JawRightBar->value()},
    };
    config.rect = roi_rect;
    return config;
}

void PaperTrackMainWindow::set_config(const PaperTrackerConfig& config)
{
    current_brightness = config.brightness;
    current_rotate_angle = config.rotate_angle;
    ui.BrightnessBar->setValue(config.brightness);
    ui.RotateImageBar->setValue(config.rotate_angle);
    ui.EnergyModeBox->setCurrentIndex(config.energy_mode);
    ui.UseFilterBox->setChecked(config.use_filter);
    ui.textEdit->setPlainText(QString::fromStdString(config.wifi_ip));
    try
    {
        ui.CheekPuffLeftBar->setValue(config.amp_map.at("cheekPuffLeft"));
        ui.CheekPuffRightBar->setValue(config.amp_map.at("cheekPuffRight"));
        ui.JawOpenBar->setValue(config.amp_map.at("jawOpen"));
        ui.JawLeftBar->setValue(config.amp_map.at("jawLeft"));
        ui.JawRightBar->setValue(config.amp_map.at("jawRight"));
        ui.MouthLeftBar->setValue(config.amp_map.at("mouthLeft"));
        ui.MouthRightBar->setValue(config.amp_map.at("mouthRight"));
        ui.TongueOutBar->setValue(config.amp_map.at("tongueOut"));
        ui.TongueUpBar->setValue(config.amp_map.at("tongueUp"));
        ui.TongueDownBar->setValue(config.amp_map.at("tongueDown"));
        ui.TongueLeftBar->setValue(config.amp_map.at("tongueLeft"));
        ui.TongueRightBar->setValue(config.amp_map.at("tongueRight"));
    } catch (std::exception& e)
    {
        LOG_ERROR("配置文件中的振幅映射错误: " + std::string(e.what()));
    }
    roi_rect = config.rect;
}

void PaperTrackMainWindow::setOnSaveConfigButtonClickedFunc(FuncWithoutArgs func)
{
    onSaveConfigButtonClickedFunc = std::move(func);
}

void PaperTrackMainWindow::onSaveConfigButtonClicked() const
{
    onSaveConfigButtonClickedFunc();
}

void PaperTrackMainWindow::setOnAmpMapChangedFunc(FuncWithoutArgs func)
{
    onAmpMapChangedFunc = std::move(func);
}

void PaperTrackMainWindow::onCheeckPuffLeftChanged(int value) const
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onCheeckPuffRightChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onJawOpenChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onJawLeftChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onJawRightChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onMouthLeftChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onMouthRightChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onTongueOutChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onTongueLeftChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onTongueRightChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onTongueUpChanged(int value)
{
    onAmpMapChangedFunc();
}

void PaperTrackMainWindow::onTongueDownChanged(int value)
{
    onAmpMapChangedFunc();
}

std::unordered_map<std::string, int> PaperTrackMainWindow::getAmpMap() const
{
    return {
        {"cheekPuffLeft", ui.CheekPuffLeftBar->value()},
        {"cheekPuffRight", ui.CheekPuffRightBar->value()},
        {"jawOpen", ui.JawOpenBar->value()},
        {"jawLeft", ui.JawLeftBar->value()},
        {"jawRight", ui.JawRightBar->value()},
        {"mouthLeft", ui.MouthLeftBar->value()},
        {"mouthRight", ui.MouthRightBar->value()},
        {"tongueOut", ui.TongueOutBar->value()},
        {"tongueUp", ui.TongueUpBar->value()},
        {"tongueDown", ui.TongueDownBar->value()},
        {"tongueLeft", ui.TongueLeftBar->value()},
        {"tongueRight", ui.TongueRightBar->value()},
    };
}

void PaperTrackMainWindow::start_image_download() const
{
    // 开始下载图片 - 修改为支持WebSocket协议
    // 检查URL格式
    const std::string& url = current_ip_;
    if (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://" ||
        url.substr(0, 5) == "ws://" || url.substr(0, 6) == "wss://") {
        // URL已经包含协议前缀，直接使用
        image_downloader->init(url);
        } else {
            // 添加默认ws://前缀
            image_downloader->init("ws://" + url);
        }
    image_downloader->start();
}

void PaperTrackMainWindow::updateWifiLabel() const
{
    if (image_downloader->isStreaming())
    {
        setWifiStatusLabel("Wifi已连接");
    } else
    {
        setWifiStatusLabel("Wifi连接失败");
    }
}

void PaperTrackMainWindow::updateSerialLabel() const
{
    if (serial_port_manager->status() == SerialStatus::OPENED)
    {
        setSerialStatusLabel("串口已连接");
    } else
    {
        setSerialStatusLabel("串口连接失败");
    }
}

cv::Mat PaperTrackMainWindow::getVideoImage() const
{
    return std::move(image_downloader->getLatestFrame());
}

void PaperTrackMainWindow::restartCheck()
{
    if (serial_port_manager->status() == SerialStatus::FAILED)
    {
        QMetaObject::invokeMethod(this, [this]() {
            setSerialStatusLabel("串口连接失败");
            serial_port_manager->stop();
            serial_port_manager->init();
        });
        while (serial_port_manager->status() == SerialStatus::CLOSED);
        if (serial_port_manager->status() == SerialStatus::OPENED)
        {
            QMetaObject::invokeMethod(this, [this]()
            {
                LOG_INFO("串口已重新连接");
                setSerialStatusLabel("串口连接成功");
            });
        }
    }
    if (!image_downloader->isStreaming())
    {
        QMetaObject::invokeMethod(this, [this]()
        {
            image_downloader->stop();
            image_downloader->start();
        });
        if (image_downloader->isStreaming())
        {
            QMetaObject::invokeMethod(this, [this]()
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                LOG_INFO("Wifi已重新连接");
                setSerialStatusLabel("Wifi连接成功");
            });
        }
    }
}