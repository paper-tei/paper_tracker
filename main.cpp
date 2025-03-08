#include <image_downloader.hpp>
#include <osc.hpp>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <video_reader.hpp>
#include <config_writer.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "main_window.hpp"
#include <curl/curl.h>
#include <coroutine>
#include <QCoreApplication>
#include <QThread>

void start_image_download(ESP32VideoStream& image_downloader, const std::string& camera_ip)
{
    if (image_downloader.isStreaming())
    {
        image_downloader.stop();
    }
    // 开始下载图片 - 修改为支持WebSocket协议
    // 检查URL格式
    const std::string& url = camera_ip;
    if (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://" ||
        url.substr(0, 5) == "ws://" || url.substr(0, 6) == "wss://") {
        // URL已经包含协议前缀，直接使用
        image_downloader.init(url);
    } else {
        // 添加默认ws://前缀
        image_downloader.init("ws://" + url);
    }
    image_downloader.start();
}

void update_ui(PaperTrackMainWindow& window)
{
    auto last_time = std::chrono::high_resolution_clock::now();
    double fps_total = 0;
    double fps_count = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while (window.is_running())
    {
        window.updateWifiLabel();
        window.updateSerialLabel();
        auto start_time = std::chrono::high_resolution_clock::now();
        try {
            if (fps_total > 1000)
            {
                fps_count = 0;
                fps_total = 0;
            }
            // caculate fps
            auto start = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(start - last_time);
            last_time = start;
            auto fps = 1000.0 / static_cast<double>(duration.count());
            fps_total += fps;
            fps_count += 1;
            fps = fps_total/fps_count;
            cv::Mat frame = window.getVideoImage();
            if (!frame.empty())
            {
                auto rotate_angle = window.getRotateAngle();
                cv::resize(frame, frame, cv::Size(280, 280), cv::INTER_NEAREST);
                int y = frame.rows / 2;
                int x = frame.cols / 2;
                auto rotate_matrix = cv::getRotationMatrix2D(cv::Point(x, y), rotate_angle, 1);
                cv::warpAffine(frame, frame, rotate_matrix, frame.size(), cv::INTER_NEAREST);
                auto roi_rect = window.getRoiRect();
                // 显示图像
                cv::rectangle(frame, roi_rect.rect, cv::Scalar(0, 255, 0), 2);
            }
            // draw rect on frame
            cv::Mat show_image;
            if (!frame.empty())
            {
                show_image = frame;
            }
            window.setVideoImage(show_image);
            // 控制帧率
        } catch (const std::exception& e) {
            // 使用Qt方式记录日志，而不是minilog
            QMetaObject::invokeMethod(&window, [&e]() {
                LOG_ERROR("错误： 视频处理异常: " + e.what());
            }, Qt::QueuedConnection);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count ();
        int delay_ms = max(0, static_cast<int>(1000.0 / min(window.get_max_fps() + 30, 60) - elapsed));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
}


void inference_image(
    PaperTrackMainWindow& window,
    Inference& inference,
    OscManager& osc_manager
)
{
    auto last_time = std::chrono::high_resolution_clock::now();

    double fps_total = 0;
    double fps_count = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while (window.is_running())
    {
        if (fps_total > 1000)
        {
            fps_count = 0;
            fps_total = 0;
        }
        // calculate fps
        auto start = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(start - last_time);
        last_time = start;
        auto fps = 1000.0 / static_cast<double>(duration.count());
        fps_total += fps;
        fps_count += 1;
        fps = fps_total/fps_count;
        // LOG_DEBUG("模型FPS： " + std::to_string(fps));

        auto start_time = std::chrono::high_resolution_clock::now();

        auto frame = window.getVideoImage();
        // 推理处理
        if (!frame.empty())
        {
            auto rotate_angle = window.getRotateAngle();
            cv::resize(frame, frame, cv::Size(280, 280), cv::INTER_NEAREST);
            int y = frame.rows / 2;
            int x = frame.cols / 2;
            auto rotate_matrix = cv::getRotationMatrix2D(cv::Point(x, y), rotate_angle, 1);
            cv::warpAffine(frame, frame, rotate_matrix, frame.size(), cv::INTER_NEAREST);
            cv::Mat infer_frame;
            infer_frame = frame.clone();

            auto roi_rect = window.getRoiRect();
            if (!roi_rect.rect.empty() && roi_rect.is_roi_end)
            {
                infer_frame = infer_frame(roi_rect.rect);
            }
            inference.inference(infer_frame);
            // 发送OSC数据
            std::vector<float> output = inference.get_output();

            if (!output.empty()) {
                window.updateCalibrationProgressBars(output, inference.getBlendShapeIndexMap());
                osc_manager.sendModelOutput(output);
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count ();
        int delay_ms = max(0, static_cast<int>(1000.0 / window.get_max_fps() - elapsed));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
}

int main(int argc, char *argv[]) {
    // Create ui application
    QApplication app(argc, argv);

    QFile qssFile("./resources/material.qss"); // 使用资源路径
    QIcon icon("./resources/window_icon.png");
    if (qssFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(qssFile.readAll());
        app.setStyleSheet(styleSheet);
        qssFile.close();
    } else {
        QMessageBox box;
        box.setWindowIcon(icon);
        box.setText("无法打开 QSS 文件");
        box.exec();
    }

    PaperTrackMainWindow window;
    window.setWindowIcon(icon);  // 设置窗口图标
    window.show();

    curl_easy_init();

    VideoReader video_reader;
    Inference inference;
    OscManager osc_manager;
    PaperTrackerConfig config;
    ConfigWriter config_writer("./config.json");

    LOG_INFO("读取配置文件中...");
    config = std::move(config_writer.get_config<PaperTrackerConfig>());
    LOG_INFO("读取配置文件成功");

    window.setBeforeStop([&osc_manager] ()
    {
        osc_manager.close();
    });

    // bound callback
    window.setOnUseFilterClickedFunc([&inference] (int value)
    {
        inference.set_use_filter(value);
    });
    window.setOnSaveConfigButtonClickedFunc([&window, &config_writer, &config] ()
    {
        LOG_INFO("保存配置中...");
        config = std::move(window.generate_config());
        if (config_writer.write_config(config))
        {
            LOG_INFO("已保存配置");
        } else
        {
            LOG_ERROR("配置文件保存失败");
        }
    });
    window.setOnAmpMapChangedFunc([&window, &inference] ()
    {
        inference.set_amp_map(window.getAmpMap());
    });

    window.set_config(config);

    // Load model
    LOG_INFO("正在加载推理模型...");
    try {
        inference.load_model("./model/model.onnx");
        LOG_INFO("模型加载完成");
    } catch (const std::exception& e) {
        // 使用Qt方式记录日志，而不是minilog
        LOG_ERROR("错误: 模型加载异常: " + e.what());
    }

    // 初始化OSC管理器
    LOG_INFO("正在初始化OSC...");
    if (osc_manager.init("127.0.0.1", 8888)) {
        osc_manager.setLocationPrefix("");
        osc_manager.setMultiplier(1.0f);
        LOG_INFO("OSC初始化成功");
    } else {
        LOG_ERROR("OSC初始化失败，请检查网络连接");
    }



    LOG_INFO("正在启动视频处理线程...");
    window.set_update_thread([ &window] ()
    {
        update_ui(window);
    });
    window.set_inference_thread([ &window, &inference, &osc_manager] ()
    {
        inference_image(window, inference, osc_manager);
    });

    // auto restart_worker = new RestartWorker(
    //     &window,
    //     &serial_port_manager,
    //     &image_downloader
    // );
    // QObject::connect(qApp, &QCoreApplication::aboutToQuit, [=]() {
    //     restart_worker->requestInterruption();
    //     restart_worker->wait();
    // });

    // restart_worker->start();

    int status = QApplication::exec();

    LOG_INFO("开始自动保存");
    if (config_writer.write_config(window.generate_config()))
    {
        LOG_INFO("已保存配置");
    } else
    {
        LOG_ERROR("配置文件保存失败");
    }

    window.stop();

    return status;
}


// #include <QCoreApplication>
// #include <QSerialPort>
// #include <QSerialPortInfo>
// #include <QDebug>
//
// class SerialHandler : public QObject {
// public:
//     SerialHandler(QObject *parent = nullptr) : QObject(parent) {
//         serialPort = new QSerialPort(this);
//
//         // 设置串口名称（根据实际情况修改）
//         serialPort->setPortName("COM6");  // Windows: COMx, Linux/macOS: /dev/ttyUSBx
//         serialPort->setBaudRate(QSerialPort::Baud115200);
//         serialPort->setDataBits(QSerialPort::Data8);
//         serialPort->setParity(QSerialPort::NoParity);
//         serialPort->setStopBits(QSerialPort::OneStop);
//         serialPort->setFlowControl(QSerialPort::NoFlowControl);
//
//         // 打开串口
//         if (!serialPort->open(QIODevice::ReadWrite)) {
//             qDebug() << "无法打开串口:" << serialPort->errorString();
//             return;
//         }
//         qDebug() << "串口已打开";
//
//         // 连接信号槽：当串口收到数据时调用 readData()
//         connect(serialPort, &QSerialPort::readyRead, this, &SerialHandler::readData);
//
//     }
//
//     ~SerialHandler() {
//         if (serialPort->isOpen()) {
//             serialPort->close();
//             qDebug() << "串口已关闭";
//         }
//     }
//
// private slots:
//     void readData() {
//         QByteArray data = serialPort->readAll();
//         qDebug() << "收到数据:" << data;
//         // 发送数据
//         // qint64 bytesWritten = serialPort->write(data);
//         // if (bytesWritten == -1) {
//         //     qDebug() << "发送失败:" << serialPort->errorString();
//         // } else {
//         //     qDebug() << "已发送" << bytesWritten << "字节";
//         // }
//     }
//
// private:
//     QSerialPort *serialPort;
// };
//
// int main(int argc, char *argv[]) {
//     QCoreApplication a(argc, argv);
//     SerialHandler serialHandler;
//     return a.exec();
// }