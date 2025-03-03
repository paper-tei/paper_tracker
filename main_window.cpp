//
// Created by JellyfishKnight on 2025/2/25.
//

#include "main_window.hpp"
#include <QMessageBox>
#include <codecvt>
#include <locale>
#include <windows.h>
#include <QTimer>
#include <QProcess>
#include <QProgressDialog>
#include <QEventLoop>
#include <QCoreApplication>
PaperTrackMainWindow::PaperTrackMainWindow(QWidget *parent)
    : QWidget(parent),
      use_user_camera(false)
{
    // 基本UI设置
    setFixedSize(848, 538);
    ui.setupUi(this);
    ui.LogText->setMaximumBlockCount(200);
    init_logger(ui.LogText);
    LOG_INFO("系统初始化中...");

    wifi_cache_file_writer = std::make_shared<WifiCacheFileWriter>("./wifi_cache.txt");
    // 初始化串口
    serial_port_manager_ = std::make_shared<SerialPortManager>();
    inference = std::make_shared<Inference>();
    osc_manager_ = std::make_shared<OscManager>();
    image_downloader_ = std::make_shared<ESP32VideoStream>();
    video_reader = std::make_shared<VideoReader>();
    // 初始化串口连接状态
    ui.SerialConnectLabel->setText("串口未连接");
    ui.WifiConnectLabel->setText("Wifi未连接");

    // 初始化亮度控制相关成员
    brightness_timer = new QTimer(this);
    brightness_timer->setSingleShot(true);
    connect(brightness_timer, &QTimer::timeout, this, &PaperTrackMainWindow::sendBrightnessValue);
    current_brightness = 0;
    // 连接UI信号槽
    connect(ui.wifi_send_Button, &QPushButton::clicked, this, &PaperTrackMainWindow::onSendButtonClicked);
    connect(ui.BrightnessBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onBrightnessChanged);
    connect(ui.FlashFirmwareButton, &QPushButton::clicked, this, &PaperTrackMainWindow::flashESP32);
    connect(ui.restart_Button, &QPushButton::clicked, this, &PaperTrackMainWindow::onRestartButtonClicked);
    // 添加输入框焦点事件处理
    ui.SSIDText->installEventFilter(this);
    ui.PasswordText->installEventFilter(this);

    // 允许Tab键在输入框之间跳转
    ui.SSIDText->setTabChangesFocus(true);
    ui.PasswordText->setTabChangesFocus(true);
    // 清除所有控件的初始焦点，确保没有文本框自动获得焦点
    setFocus();
    // 设置设备状态回调
    serial_port_manager_->setDeviceStatusCallback([this](const std::string& ip, int brightness, int power, int version) {
        // 使用Qt的线程安全方式更新UI
        QMetaObject::invokeMethod(this, [this, ip, brightness, power, version]() {
            // 只在 IP 地址变化时更新显示
            if (current_ip_ != ip)
            {
                current_ip_ = ip;
                // 更新IP地址显示，添加 http:// 前缀
                ui.textEdit->setText("http://" + QString::fromStdString(ip));
                LOG_INFO(QString("IP地址已更新: http://%1").arg(QString::fromStdString(ip)));

                if (!use_user_camera)
                {
                    wifi_cache_file_writer->write_wifi_config(current_ip_);
                    start_image_download();
                }
            }
            // 可以添加其他状态更新的日志，如果需要的话
        }, Qt::QueuedConnection);
    });
    // 添加ROI事件
    ROIEventFilter *roiFilter = new ROIEventFilter([this] (QRect rect, bool isEnd)
    {
        is_roi_end = isEnd;
        roi_rect = cv::Rect(rect.x(), rect.y(), rect.width(), rect.height());
        // if end point is out of image, move rect
        if (roi_rect.x < 0)
        {
            roi_rect.width += rect.x();
            roi_rect.x = 0;
        }
        if (roi_rect.y < 0)
        {
            roi_rect.height += rect.y();
            roi_rect.y = 0;
        }
        // if roi is bigger than image, resize rect
        if (roi_rect.x + roi_rect.width > 361)
        {
            roi_rect.width = 361 - roi_rect.x;
        }
        if (roi_rect.y + roi_rect.height > 251)
        {
            roi_rect.height = 251 - roi_rect.y;
        }
    },ui.ImageLabel);
    ui.ImageLabel->installEventFilter(roiFilter);

    // 初始化页面导航
    bound_pages();

    // 异步加载视频
    if (use_user_camera)
    {
        LOG_INFO("正在加载视频...");
        auto video_future = std::async(std::launch::async, [this]() {
            try {
                video_reader->open_video("D:/Babble/test_video.mkv");
                if (!video_reader->is_opened()) {
                    // 使用Qt方式记录日志，而不是minilog
                    QMetaObject::invokeMethod(this, [this]() {
                        LOG_ERROR("错误: 视频打开失败");
                    }, Qt::QueuedConnection);
                    return ;
                }
            } catch (const std::exception& e) {
                // 使用Qt方式记录日志，而不是minilog
                QString errorMsg = QString("视频加载异常: %1").arg(e.what());
                QMetaObject::invokeMethod(this, [this, errorMsg]() {
                    LOG_ERROR("错误: " + errorMsg);
                }, Qt::QueuedConnection);
            }
        });
    }

    // 加载推理模型
    LOG_INFO("正在加载推理模型...");
    try {
        inference->load_model("./model/model.onnx");
        LOG_INFO("模型加载完成");
    } catch (const std::exception& e) {
        // 使用Qt方式记录日志，而不是minilog
        LOG_ERROR(QString("错误: 模型加载异常: %1").arg(e.what()));
    }

    // 初始化OSC管理器
    LOG_INFO("正在初始化OSC...");
    if (osc_manager_->init("127.0.0.1", 8888)) {
        osc_manager_->setLocationPrefix("");
        osc_manager_->setMultiplier(1.0f);
        LOG_INFO("OSC初始化成功");
    } else {
        LOG_ERROR("OSC初始化失败，请检查网络连接");
    }

    // 启动串口
    LOG_INFO("正在初始化串口...");
    serial_port_manager_->start();
    initBlendShapeIndexMap();
    LOG_INFO("初始化ARKit模型输出映射表完成");
    LOG_INFO("系统初始化完成");

    LOG_INFO("等待串口状态响应");
    while (serial_port_manager_->status() == SerialStatus::CLOSED) {}
    LOG_INFO("串口状态响应完毕");

    if (serial_port_manager_->status() == SerialStatus::FAILED && !use_user_camera)
    {
        ui.SerialConnectLabel->setText("串口连接失败");
        LOG_WARN("串口未连接，尝试从wifi缓存中读取地址...");
        auto ip = wifi_cache_file_writer->try_get_wifi_config();
        if (ip.has_value() && !ip.value().empty())
        {
            LOG_INFO("从wifi缓存中读取地址成功");
            current_ip_ = ip.value();
            auto esp32_future = std::async(std::launch::async, [this, ip]()
            {
                start_image_download();
            });
        } else
        {
            QMessageBox msgBox;
            msgBox.setText("未找到WiFi配置信息，请使用串口进行首次配置");
            msgBox.exec();
        }
    } else
    {
        ui.SerialConnectLabel->setText("串口连接成功");
    }


    // 启动视频显示线程
    LOG_INFO("正在启动视频处理线程...");
    show_video_thread = std::thread([this]() {
        cv::Mat frame;
        while (!window_closed) {
            try {
                if (use_user_camera)
                {
                    if (!video_reader->is_opened()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        continue;
                    }
                    // 获取视频帧
                     frame = video_reader->get_image();
                } else {
                    if (!image_buffer_queue.empty())
                    {
                        frame = std::move(image_buffer_queue.front());
                        image_buffer_queue.pop();
                    }
                }
                bool image_captured = true;
                if (frame.empty()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    image_captured = false;
                }


                // 推理处理
                if (image_captured)
                {
                    cv::resize(frame, frame, cv::Size(361, 251));
                    cv::Mat infer_frame;
                    infer_frame = frame.clone();
                    if (!roi_rect.empty() && is_roi_end)
                    {
                        infer_frame = infer_frame(roi_rect);
                    }
                    // 显示图像
                    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

                    cv::rectangle(frame, roi_rect, cv::Scalar(0, 255, 0), 2);
                    inference->inference(infer_frame);
                    // 发送OSC数据
                    std::vector<float> output = inference->get_output();
                    if (!output.empty()) {
                        osc_manager_->sendModelOutput(output);
                        updateCalibrationProgressBars(output);
                    }

                }
                // draw rect on frame
                QImage qimage;
                if (image_captured)
                {
                    qimage = QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
                } else
                {
                    // default image which displays: "没有图像载入“
                    cv::Mat default_frame = cv::Mat::zeros(251, 361, CV_8UC3);
                    default_frame = cv::Scalar(255, 255, 255);
                    cv::putText(default_frame, "No Image Loaded", cv::Point(10, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
                    qimage = QImage(default_frame.data, default_frame.cols, default_frame.rows, default_frame.step, QImage::Format_RGB888).copy();
                }

                // 使用Qt的线程安全方式更新UI
                QMetaObject::invokeMethod(this, [this, qimage]() {
                    ui.ImageLabel->setPixmap(QPixmap::fromImage(qimage));
                    ui.ImageLabel->setScaledContents(true);
                    ui.ImageLabel->update();
                }, Qt::QueuedConnection);
                // 控制帧率
                cv::waitKey(13);
                // std::this_thread::sleep_for(std::chrono::milliseconds(10)); // ~30fps
            } catch (const std::exception& e) {
                // 使用Qt方式记录日志，而不是minilog
                QMetaObject::invokeMethod(this, [e]() {
                LOG_ERROR(QString("错误： 视频处理异常: %1").arg(e.what()));
                }, Qt::QueuedConnection);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    });

}

PaperTrackMainWindow::~PaperTrackMainWindow() {
    // 安全关闭
    LOG_INFO("正在关闭系统...");
    window_closed = true;
    image_downloader_->stop();
    if (brightness_timer) {
        brightness_timer->stop();
        delete brightness_timer;
    }
    // 等待线程结束
    if (show_video_thread.joinable()) {
        show_video_thread.join();
    }

    // 其他清理工作
    LOG_INFO("系统已安全关闭");
}
// 新增的重启按钮处理函数
void PaperTrackMainWindow::onRestartButtonClicked() {
    // 调用restartESP32函数
    restartESP32();
}
// 新增的重启ESP32函数实现
void PaperTrackMainWindow::restartESP32() {
    // 记录操作
    LOG_INFO("准备重启ESP32设备...");
    serial_port_manager_->stop();

    try {
        // 从SerialPortManager获取端口名
        std::string port = getPortFromSerialManager();
        if (port.empty()) {
            port = "COM2"; // 默认端口
        }

        LOG_INFO("使用端口: " + QString::fromStdString(port));

        // 构造完整的命令路径
        QString appDir = QCoreApplication::applicationDirPath();
        QString esptoolPath = "\"" + appDir + "/esptool.exe\"";

        // 构造重启命令 - 只执行重启操作
        QString commandStr = QString("\"%1/esptool.exe\" --port %2 run")
            .arg(appDir)
            .arg(port.c_str());
        LOG_INFO("执行重启命令: " + commandStr);

        // 创建进度窗口
        QProgressDialog progress("正在重启设备，请稍候...", "取消", 0, 0, this);
        progress.setWindowTitle("设备重启");
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        progress.setMaximum(0); // 设置为0表示未知进度

        // 使用QProcess执行命令
        QProcess process;

        // 捕获标准输出和错误输出
        connect(&process, &QProcess::readyReadStandardOutput, [&]() {
            QString output = process.readAllStandardOutput();
            ui.LogText->appendPlainText(output.trimmed());
        });

        connect(&process, &QProcess::readyReadStandardError, [&]() {
            QString error = process.readAllStandardError();
            LOG_ERROR("错误： " + error.trimmed());
        });

        // 绑定取消按钮
        connect(&progress, &QProgressDialog::canceled, [&]() {
            process.kill();
            LOG_INFO("用户取消了设备重启");
        });

        // 启动进程
        process.start(commandStr);

        // 等待进程启动
        if (!process.waitForStarted(3000)) {
            LOG_ERROR("无法启动esptool.exe: " + process.errorString());
            QMessageBox::critical(this, "启动失败", "无法启动esptool.exe: " + process.errorString());
            return;
        }

        LOG_INFO("重启进程已启动，请等待完成...");

        // 使用事件循环等待进程完成，同时保持UI响应
        QEventLoop loop;
        connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
        loop.exec();

        // 进程完成
        progress.setValue(100);

        // 处理结果
        if (process.exitCode() == 0) {
            LOG_INFO("设备重启成功！");
            QMessageBox::information(this, "重启完成", "ESP32设备重启成功！");
        } else {
            LOG_ERROR("设备重启失败，退出码: " + QString::number(process.exitCode()));
            QMessageBox::critical(this, "重启失败", "ESP32设备重启失败，请检查连接！");
        }

        // 重新初始化和启动串口
        serial_port_manager_->init();
        serial_port_manager_->start();
    } catch (const std::exception& e) {
        LOG_ERROR("重启过程发生异常: " + QString(e.what()));
        QMessageBox::critical(this, "错误", "重启过程中发生异常: " + QString(e.what()));
    }
}
void PaperTrackMainWindow::bound_pages() {
    // 页面导航逻辑
    connect(ui.MainPageButton, &QPushButton::clicked, [this] {
        ui.stackedWidget->setCurrentIndex(0);
    });
    connect(ui.CalibrationPageButton, &QPushButton::clicked, [this] {
        ui.stackedWidget->setCurrentIndex(1);
    });
}

void PaperTrackMainWindow::onSendButtonClicked() {
    // 获取SSID和密码
    QString ssid = ui.SSIDText->toPlainText();
    QString password = ui.PasswordText->toPlainText();

    // 输入验证
    if (ssid == "请输入SSID" || ssid.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入有效的SSID");
        return;
    }

    if (password == "请输入密码" || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入有效的密码");
        return;
    }

    // 构建并发送数据包
    std::string packet = "A2SSID" + ssid.toStdString() + "PWD" + password.toStdString() + "B2";
    serial_port_manager_->write_data(packet);

    // 记录操作
    LOG_INFO("已发送WiFi配置: SSID=" + ssid + ", PWD=" + password);

    LOG_INFO("开始自动重启ESP32...");
    restartESP32();
}

void PaperTrackMainWindow::onBrightnessChanged(int value) {
    // 更新当前亮度值
    current_brightness = value;

    // 重置定时器，如果用户500毫秒内没有再次改变值，就发送数据包
    brightness_timer->start(100);
}
// 添加新的发送亮度值函数
void PaperTrackMainWindow::sendBrightnessValue() {
    // 发送亮度控制命令 - 确保亮度值为三位数字
    std::string brightness_str = std::to_string(current_brightness);
    // 补齐三位数字，前面加0
    while (brightness_str.length() < 3) {
        brightness_str = "0" + brightness_str;
    }
    std::string packet = "A6" + brightness_str + "B6";
    serial_port_manager_->write_data(packet);

    // 记录操作
    LOG_INFO("已设置亮度: " + QString::number(current_brightness));
}
// 获取串口端口名称
std::string PaperTrackMainWindow::getPortFromSerialManager() {
    // 从SerialPortManager获取COM端口名称
    return serial_port_manager_->getCurrentPort();
}
// 刷写ESP32固件
void PaperTrackMainWindow::flashESP32() {
    // 记录操作
    LOG_INFO("准备刷写ESP32固件...");
    serial_port_manager_->stop();
    // 不再调用stop()，而是手动关闭程序持有的串口句柄
    // 这样可以释放COM端口而不会导致其他部分的问题
    try {
        // 从SerialPortManager获取端口名
        std::string port = getPortFromSerialManager();
        if (port.empty()) {
            port = "COM2"; // 默认端口
        }
        LOG_INFO("使用端口: " + QString::fromStdString(port));
        // 构造完整的命令路径
        QString appDir = QCoreApplication::applicationDirPath();
        QString esptoolPath = "\"" + appDir + "/esptool.exe\"";
        QString bootloaderPath = "\"" + appDir + "/bootloader.bin\"";
        QString partitionPath = "\"" + appDir + "/partition-table.bin\"";
        QString firmwarePath = "\"" + appDir + "/WIFI.bin\"";

        // 构造命令
        QString commandStr = QString("%1 --chip ESP32-S3 --port %2 --baud 921600 --before default_reset --after hard_reset write_flash 0x0000 %3 0x8000 %4 0x10000 %5")
            .arg(esptoolPath)
            .arg(port.c_str())
            .arg(bootloaderPath)
            .arg(partitionPath)
            .arg(firmwarePath);
        LOG_INFO("执行命令: " + commandStr);

        // 创建进度窗口
        QProgressDialog progress("正在刷写固件，请稍候...", "取消", 0, 0, this);
        progress.setWindowTitle("固件刷写");
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        progress.setMaximum(0); // 设置为0表示未知进度

        // 使用QProcess执行命令
        QProcess process;

        // 捕获标准输出和错误输出
        connect(&process, &QProcess::readyReadStandardOutput, [&]() {
            QString output = process.readAllStandardOutput();
            ui.LogText->appendPlainText(output.trimmed());
        });

        connect(&process, &QProcess::readyReadStandardError, [&]() {
            QString error = process.readAllStandardError();
            LOG_ERROR("错误: " + error.trimmed());
        });

        // 绑定取消按钮
        connect(&progress, &QProgressDialog::canceled, [&]() {
            process.kill();
            LOG_WARN("用户取消了固件刷写");
        });

        // 启动进程
        process.start(commandStr);

        // 等待进程启动
        if (!process.waitForStarted(3000)) {
            LOG_ERROR("无法启动esptool.exe: " + process.errorString());
            QMessageBox::critical(this, "启动失败", "无法启动esptool.exe: " + process.errorString());
            return;
        }
        LOG_INFO("刷写进程已启动，请等待完成...");

        // 使用事件循环等待进程完成，同时保持UI响应
        QEventLoop loop;
        connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
        loop.exec();

        // 进程完成
        progress.setValue(100);

        // 处理结果
        if (process.exitCode() == 0) {
            LOG_INFO("固件刷写成功！");
            QMessageBox::information(this, "刷写完成", "ESP32固件刷写成功！");
        } else {
            LOG_ERROR("固件刷写失败，退出码: " + QString::number(process.exitCode()));
            QMessageBox::critical(this, "刷写失败", "ESP32固件刷写失败，请检查连接和固件文件！");
        }

        // 重新启动程序
        // QString appPath = QCoreApplication::applicationFilePath();
        // QProcess::startDetached(appPath);
        // QApplication::quit();
        serial_port_manager_->init();
        serial_port_manager_->start();
    } catch (const std::exception& e) {
        LOG_ERROR("发生异常: " + QString(e.what()));
        QMessageBox::critical(this, "错误", "刷写过程中发生异常: " + QString(e.what()));
    }
}
// 添加事件过滤器实现
bool PaperTrackMainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // 处理焦点获取事件
    if (event->type() == QEvent::FocusIn) {
        if (obj == ui.SSIDText) {
            if (ui.SSIDText->toPlainText() == "请输入SSID") {
                ui.SSIDText->setPlainText("");
            }
        } else if (obj == ui.PasswordText) {
            if (ui.PasswordText->toPlainText() == "请输入密码") {
                ui.PasswordText->setPlainText("");
            }
        }
    }

    // 处理焦点失去事件
    if (event->type() == QEvent::FocusOut) {
        if (obj == ui.SSIDText) {
            if (ui.SSIDText->toPlainText().isEmpty()) {
                ui.SSIDText->setPlainText("请输入SSID");
            }
        } else if (obj == ui.PasswordText) {
            if (ui.PasswordText->toPlainText().isEmpty()) {
                ui.PasswordText->setPlainText("请输入密码");
            }
        }
    }

    // 继续事件处理
    return QWidget::eventFilter(obj, event);
}

void PaperTrackMainWindow::onUseUserCameraClicked(int value)
{
    use_user_camera = value;
}

// 初始化ARKit模型输出的映射表
void PaperTrackMainWindow::initBlendShapeIndexMap() {
    // 定义所有ARKit模型输出名称及其索引
    const std::vector<std::string> blendShapes = {
        "cheekPuffLeft", "cheekPuffRight",
        "cheekSuckLeft", "cheekSuckRight",
        "jawOpen", "jawForward", "jawLeft", "jawRight",
        "noseSneerLeft", "noseSneerRight",
        "mouthFunnel", "mouthPucker",
        "mouthLeft", "mouthRight",
        "mouthRollUpper", "mouthRollLower",
        "mouthShrugUpper", "mouthShrugLower",
        "mouthClose",
        "mouthSmileLeft", "mouthSmileRight",
        "mouthFrownLeft", "mouthFrownRight",
        "mouthDimpleLeft", "mouthDimpleRight",
        "mouthUpperUpLeft", "mouthUpperUpRight",
        "mouthLowerDownLeft", "mouthLowerDownRight",
        "mouthPressLeft", "mouthPressRight",
        "mouthStretchLeft", "mouthStretchRight",
        "tongueOut", "tongueUp", "tongueDown",
        "tongueLeft", "tongueRight",
        "tongueRoll", "tongueBendDown",
        "tongueCurlUp", "tongueSquish",
        "tongueFlat", "tongueTwistLeft",
        "tongueTwistRight"
    };

    // 构建映射
    for (size_t i = 0; i < blendShapes.size(); ++i) {
        blendShapeIndexMap[blendShapes[i]] = i;
    }
}

// 根据模型输出更新校准页面的进度条
void PaperTrackMainWindow::updateCalibrationProgressBars(const std::vector<float>& output) {
    if (output.empty() || ui.stackedWidget->currentIndex() != 1) {
        // 如果输出为空或者当前不在校准页面，则不更新
        return;
    }

    // 使用Qt的线程安全方式更新UI
    QMetaObject::invokeMethod(this, [this, output]() {
        // 将值缩放到0-100范围内用于进度条显示
        auto scaleValue = [](float value) -> int {
            // 将值限制在0-1.0范围内，然后映射到0-100
            return static_cast<int>(value * 100);
        };

        // 更新各个进度条
        // 注意：这里假设输出数组中的索引与ARKit模型输出的顺序一致

        // 脸颊
        if (blendShapeIndexMap.count("cheekPuffLeft") && blendShapeIndexMap["cheekPuffLeft"] < output.size()) {
            ui.CheekPullLeftValue->setValue(scaleValue(output[blendShapeIndexMap["cheekPuffLeft"]]));
        }

        if (blendShapeIndexMap.count("cheekPuffRight") && blendShapeIndexMap["cheekPuffRight"] < output.size()) {
            ui.CheekPullRightValue->setValue(scaleValue(output[blendShapeIndexMap["cheekPuffRight"]]));
        }

        // 下巴
        if (blendShapeIndexMap.count("jawOpen") && blendShapeIndexMap["jawOpen"] < output.size()) {
            ui.JawOpenValue->setValue(scaleValue(output[blendShapeIndexMap["jawOpen"]]));
        }

        if (blendShapeIndexMap.count("jawLeft") && blendShapeIndexMap["jawLeft"] < output.size()) {
            ui.JawLeftValue->setValue(scaleValue(output[blendShapeIndexMap["jawLeft"]]));
        }

        if (blendShapeIndexMap.count("jawRight") && blendShapeIndexMap["jawRight"] < output.size()) {
            ui.JawRightValue->setValue(scaleValue(output[blendShapeIndexMap["jawRight"]]));
        }

        // 嘴巴
        if (blendShapeIndexMap.count("mouthLeft") && blendShapeIndexMap["mouthLeft"] < output.size()) {
            ui.MouthLeftValue->setValue(scaleValue(output[blendShapeIndexMap["mouthLeft"]]));
        }

        if (blendShapeIndexMap.count("mouthRight") && blendShapeIndexMap["mouthRight"] < output.size()) {
            ui.MouthRightValue->setValue(scaleValue(output[blendShapeIndexMap["mouthRight"]]));
        }

        // 舌头
        if (blendShapeIndexMap.count("tongueOut") && blendShapeIndexMap["tongueOut"] < output.size()) {
            ui.TongueOutValue->setValue(scaleValue(output[blendShapeIndexMap["tongueOut"]]));
        }

        if (blendShapeIndexMap.count("tongueUp") && blendShapeIndexMap["tongueUp"] < output.size()) {
            ui.TongueUpValue->setValue(scaleValue(output[blendShapeIndexMap["tongueUp"]]));
        }

        if (blendShapeIndexMap.count("tongueDown") && blendShapeIndexMap["tongueDown"] < output.size()) {
            ui.TongueDownValue->setValue(scaleValue(output[blendShapeIndexMap["tongueDown"]]));
        }

        if (blendShapeIndexMap.count("tongueLeft") && blendShapeIndexMap["tongueLeft"] < output.size()) {
            ui.TongueLeftValue->setValue(scaleValue(output[blendShapeIndexMap["tongueLeft"]]));
        }

        if (blendShapeIndexMap.count("tongueRight") && blendShapeIndexMap["tongueRight"] < output.size()) {
            ui.TongueRightValue->setValue(scaleValue(output[blendShapeIndexMap["tongueRight"]]));
        }
    }, Qt::QueuedConnection);
}

void PaperTrackMainWindow::start_image_download()
{
    image_downloader_->stop();
    image_downloader_->init("http://" + current_ip_, [this] (const cv::Mat& image)
    {
        if (image_buffer_queue.size() > 0)
        {
            return ;
        }
        image_buffer_queue.push(image.clone());
    });
    image_downloader_->start();

    if (image_downloader_->isStreaming())
    {
        ui.WifiConnectLabel->setText("Wifi连接成功");
    }
}