//
// Created by JellyfishKnight on 2025/2/25.
//

#include "main_window.hpp"
#include <QMessageBox>

PaperTrackMainWindow::PaperTrackMainWindow(QWidget *parent)
    : QWidget(parent),
      serial_port_manager_(ui.LogText),
      use_user_camera(false)
{
    // 基本UI设置
    setFixedSize(848, 538);
    ui.setupUi(this);
    ui.LogText->setMaximumBlockCount(200);
    ui.LogText->appendPlainText("系统初始化中...");

    // 连接UI信号槽
    connect(ui.pushButton, &QPushButton::clicked, this, &PaperTrackMainWindow::onSendButtonClicked);
    connect(ui.BrightnessBar, &QScrollBar::valueChanged, this, &PaperTrackMainWindow::onBrightnessChanged);
        
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
    ui.LogText->appendPlainText("正在加载视频...");

    if (use_user_camera)
    {
        auto video_future = std::async(std::launch::async, [this]() {
            try {
                video_reader.open_video("D:/Babble/test_video.mkv");
                if (!video_reader.is_opened()) {
                    // 使用Qt方式记录日志，而不是minilog
                    QMetaObject::invokeMethod(this, [this]() {
                        ui.LogText->appendPlainText("错误: 视频打开失败");
                    }, Qt::QueuedConnection);
                    return ;
                }
            } catch (const std::exception& e) {
                // 使用Qt方式记录日志，而不是minilog
                QString errorMsg = QString("视频加载异常: %1").arg(e.what());
                QMetaObject::invokeMethod(this, [this, errorMsg]() {
                    ui.LogText->appendPlainText("错误: " + errorMsg);
                }, Qt::QueuedConnection);
            }
        });
    } else
    {
        auto esp32_future = std::async(std::launch::async, [this]()
        {
            image_downloader_.init(esp32_ip_address, [this] (const cv::Mat& image)
            {
                if (image_buffer_queue.size() > 0)
                {
                    return ;
                }
                image_buffer_queue.push(image.clone());


            });
        });
    }

    // 加载推理模型
    ui.LogText->appendPlainText("正在加载推理模型...");
    try {
        inference.load_model("./model/model.onnx");
        ui.LogText->appendPlainText("模型加载完成");
    } catch (const std::exception& e) {
        // 使用Qt方式记录日志，而不是minilog
        ui.LogText->appendPlainText(QString("错误: 模型加载异常: %1").arg(e.what()));
    }

    // 初始化OSC管理器
    ui.LogText->appendPlainText("正在初始化OSC...");
    if (osc_manager_.init("127.0.0.1", 8888)) {
        osc_manager_.setLocationPrefix("");
        osc_manager_.setMultiplier(1.0f);
        ui.LogText->appendPlainText("OSC初始化成功");
    } else {
        ui.LogText->appendPlainText("OSC初始化失败，请检查网络连接");
    }

    image_downloader_.start();
    // 启动视频显示线程
    ui.LogText->appendPlainText("正在启动视频处理线程...");
    show_video_thread = std::thread([this]() {
        cv::Mat frame;
        while (!window_closed) {
            try {
                if (use_user_camera)
                {
                    if (!video_reader.is_opened()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        continue;
                    }
                    // 获取视频帧
                     frame = video_reader.get_image();
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
                    inference.inference(infer_frame);
                    // 发送OSC数据
                    std::vector<float> output = inference.get_output();
                    if (!output.empty()) {
                        osc_manager_.sendModelOutput(output);
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
                cv::waitKey(33);
                // std::this_thread::sleep_for(std::chrono::milliseconds(10)); // ~30fps
            } catch (const std::exception& e) {
                // 使用Qt方式记录日志，而不是minilog
                QString errorMsg = QString("视频处理异常: %1").arg(e.what());
                QMetaObject::invokeMethod(this, [this, errorMsg]() {
                    ui.LogText->appendPlainText("错误: " + errorMsg);
                }, Qt::QueuedConnection);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    });

    // 启动串口
    ui.LogText->appendPlainText("正在初始化串口...");
    serial_port_manager_.start();
    ui.LogText->appendPlainText("系统初始化完成");
}

PaperTrackMainWindow::~PaperTrackMainWindow() {
    // 安全关闭
    ui.LogText->appendPlainText("正在关闭系统...");
    window_closed = true;
    image_downloader_.stop();

    // 等待线程结束
    if (show_video_thread.joinable()) {
        show_video_thread.join();
    }

    // 其他清理工作
    ui.LogText->appendPlainText("系统已安全关闭");
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
    serial_port_manager_.write_data(packet);

    // 记录操作
    ui.LogText->appendPlainText("已发送WiFi配置: SSID=" + ssid + ", PWD=" + password);
}

void PaperTrackMainWindow::onBrightnessChanged(int value) {
    // 发送亮度控制命令
    std::string packet = "A6" + std::to_string(value) + "B6";
    serial_port_manager_.write_data(packet);

    // 记录操作
    ui.LogText->appendPlainText("已设置亮度: " + QString::number(value));
}

void PaperTrackMainWindow::onUseUserCameraClicked(int value)
{
    use_user_camera = value;
}