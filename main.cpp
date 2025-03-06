#include <image_downloader.hpp>
#include <osc.hpp>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <video_reader.hpp>
#include <config_writer.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "main_window.hpp"
#include <curl/curl.h>

void start_image_download(PaperTrackMainWindow &window, ESP32VideoStream& image_downloader, const std::string& camera_ip)
{
    if (image_downloader.isStreaming())
    {
        image_downloader.stop();
    }
    // 开始下载图片
    image_downloader.init(camera_ip);
    image_downloader.start();

    if (image_downloader.isStreaming())
    {
        window.setWifiStatusLabel("Wifi已连接");
    }
}

void restart_esp32(SerialPortManager& serial_port_manager, PaperTrackMainWindow& window)
{
    // 记录操作
    LOG_INFO("准备重启ESP32设备...");
    serial_port_manager.stop();

    try {
        // 从SerialPortManager获取端口名
        std::string port = serial_port_manager.FindEsp32S3Port();
        if (port.empty()) {
            port = "COM2"; // 默认端口
        }

        LOG_INFO("使用端口: " + port);

        // 构造完整的命令路径
        QString appDir = QCoreApplication::applicationDirPath();

        // 构造重启命令 - 只执行重启操作
        QString commandStr = QString("\"%1/esptool.exe\" --port %2 run")
            .arg(appDir, port.c_str());
        LOG_INFO("执行重启命令: " + commandStr.toStdString());

        // 创建进度窗口
        QProgressDialog progress("正在重启设备，请稍候...", "取消", 0, 0, &window);
        progress.setWindowTitle("设备重启");
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        progress.setMaximum(0); // 设置为0表示未知进度

        // 使用QProcess执行命令
        QProcess process;

        // 捕获标准输出和错误输出
        PaperTrackMainWindow::connect(&process, &QProcess::readyReadStandardOutput, [&]() {
            QString output = process.readAllStandardOutput();
            LOG_INFO(output.trimmed().toStdString());
        });

        PaperTrackMainWindow::connect(&process, &QProcess::readyReadStandardError, [&]() {
            QString error = process.readAllStandardError();
            LOG_ERROR("错误： " + error.trimmed().toStdString());
        });

        // 绑定取消按钮
        PaperTrackMainWindow::connect(&progress, &QProgressDialog::canceled, [&]() {
            process.kill();
            LOG_INFO("用户取消了设备重启");
        });

        // 启动进程
        process.start(commandStr);

        // 等待进程启动
        if (!process.waitForStarted(3000)) {
            LOG_ERROR("无法启动esptool.exe: " + process.errorString().toStdString());
            QMessageBox::critical(&window, "启动失败", "无法启动esptool.exe: " + process.errorString());
            return;
        }

        LOG_INFO("重启进程已启动，请等待完成...");

        // 使用事件循环等待进程完成，同时保持UI响应
        QEventLoop loop;
        PaperTrackMainWindow::connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
        loop.exec();

        // 进程完成
        progress.setValue(100);

        // 处理结果
        if (process.exitCode() == 0) {
            LOG_INFO("设备重启成功！");
            QMessageBox::information(&window, "重启完成", "ESP32设备重启成功！");
        } else {
            LOG_ERROR("设备重启失败，退出码: " + std::to_string(process.exitCode()));
            QMessageBox::critical(&window, "重启失败", "ESP32设备重启失败，请检查连接！");
        }

        // 重新初始化和启动串口
        serial_port_manager.init();
        serial_port_manager.start();
    } catch (const std::exception& e) {
        LOG_ERROR("重启过程发生异常: " + e.what());
        QMessageBox::critical(&window, "错误", "重启过程中发生异常: " + QString(e.what()));
    }
}

void flash_esp32(PaperTrackMainWindow& window, SerialPortManager& serial_port_manager)
{
        // 记录操作
    LOG_INFO("准备刷写ESP32固件...");
    serial_port_manager.stop();
    // 不再调用stop()，而是手动关闭程序持有的串口句柄
    // 这样可以释放COM端口而不会导致其他部分的问题
    try {
        // 从SerialPortManager获取端口名
        std::string port = serial_port_manager.FindEsp32S3Port();
        if (port.empty()) {
            port = "COM2"; // 默认端口
        }
        LOG_INFO("使用端口: " + port);
        // 构造完整的命令路径
        QString appDir = QCoreApplication::applicationDirPath();
        QString esptoolPath = "\"" + appDir + "/esptool.exe\"";
        QString bootloaderPath = "\"" + appDir + "/bootloader.bin\"";
        QString partitionPath = "\"" + appDir + "/partition-table.bin\"";
        QString firmwarePath = "\"" + appDir + "/face_tracker.bin\"";

        // 构造命令
        QString commandStr = QString("%1 --chip ESP32-S3 --port %2 --baud 921600 --before default_reset --after hard_reset write_flash 0x0000 %3 0x8000 %4 0x10000 %5")
            .arg(esptoolPath, port.c_str(), bootloaderPath, partitionPath, firmwarePath);
        LOG_INFO("执行命令: " + commandStr.toStdString());

        // 创建进度窗口
        QProgressDialog progress("正在刷写固件，请稍候...", "取消", 0, 0, &window);
        progress.setWindowTitle("固件刷写");
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        progress.setMaximum(0); // 设置为0表示未知进度

        // 使用QProcess执行命令
        QProcess process;

        // 捕获标准输出和错误输出
        PaperTrackMainWindow::connect(&process, &QProcess::readyReadStandardOutput, [&]() {
            QString output = process.readAllStandardOutput();
            LOG_INFO(output.trimmed().toStdString());
        });

        PaperTrackMainWindow::connect(&process, &QProcess::readyReadStandardError, [&]() {
            QString error = process.readAllStandardError();
            LOG_ERROR("错误: " + error.trimmed().toStdString());
        });

        // 绑定取消按钮
        PaperTrackMainWindow::connect(&progress, &QProgressDialog::canceled, [&]() {
            process.kill();
            LOG_WARN("用户取消了固件刷写");
        });

        // 启动进程
        process.start(commandStr);

        // 等待进程启动
        if (!process.waitForStarted(3000)) {
            LOG_ERROR("无法启动esptool.exe: " + process.errorString().toStdString());
            QMessageBox::critical(&window, "启动失败", "无法启动esptool.exe: " + process.errorString());
            return;
        }
        LOG_INFO("刷写进程已启动，请等待完成...");

        // 使用事件循环等待进程完成，同时保持UI响应
        QEventLoop loop;
        PaperTrackMainWindow::connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
        loop.exec();

        // 进程完成
        progress.setValue(100);

        // 处理结果
        if (process.exitCode() == 0) {
            LOG_INFO("固件刷写成功！");
            QMessageBox::information(&window, "刷写完成", "ESP32固件刷写成功！");
        } else {
            LOG_ERROR("固件刷写失败，退出码: " + std::to_string(process.exitCode()));
            QMessageBox::critical(&window, "刷写失败", "ESP32固件刷写失败，请检查连接和固件文件！");
        }

        // 重新启动程序
        // QString appPath = QCoreApplication::applicationFilePath();
        // QProcess::startDetached(appPath);
        // QApplication::quit();
        serial_port_manager.init();
        serial_port_manager.start();
    } catch (const std::exception& e) {
        LOG_ERROR("发生异常: " + e.what());
        QMessageBox::critical(&window, "错误", "刷写过程中发生异常: " + QString(e.what()));
    }
}

void update_ui(
    PaperTrackMainWindow& window,
    ESP32VideoStream& image_downloader,
    Inference& inference,
    OscManager& osc_manager
)
{
    cv::Mat frame;
    auto last_time = std::chrono::high_resolution_clock::now();
    double fps_total = 0;
    double fps_count = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    static double max_rate = 15;
    while (window.is_running())
    {
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
            auto fps = 1000.0 / duration.count();
            fps_total += fps;
            fps_count += 1;
            fps = fps_total/fps_count;
            frame = image_downloader.getLatestFrame();
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
            //    cv::putText(show_image, "FPS: " + std::to_string(fps), cv::Point(10, 50), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 0), 2);
            } else
            {
                // default image which displays: "没有图像载入“
                show_image = cv::Mat::zeros(280, 280, CV_8UC3);
                show_image = cv::Scalar(255, 255, 255);
                cv::putText(show_image, "No Image", cv::Point(10, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
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
        int delay_ms = max(0, static_cast<int>(1000.0 / (window.get_max_fps()+30) - elapsed));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
}


void inference_image(
    PaperTrackMainWindow& window,
    ESP32VideoStream& image_downloader,
    Inference& inference,
    OscManager& osc_manager
)
{
    cv::Mat frame;
    auto last_time = std::chrono::high_resolution_clock::now();

    double fps_total = 0;
    double fps_count = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    static double max_rate = 15;
    while (window.is_running())
    {
        if (fps_total > 1000)
        {
            fps_count = 0;
            fps_total = 0;
        }
        // caculate fps
        auto start = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(start - last_time);
        last_time = start;
        auto fps = 1000.0 / duration.count();
        fps_total += fps;
        fps_count += 1;
        fps = fps_total/fps_count;
        LOG_DEBUG("模型FPS： " + std::to_string(fps));

        auto start_time = std::chrono::high_resolution_clock::now();

        frame = image_downloader.getLatestFrame();
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
    SerialPortManager serial_port_manager;
    ESP32VideoStream image_downloader;
    Inference inference;
    OscManager osc_manager;
    PaperTrackerConfig config;
    ConfigWriter config_writer("./config.json");

    window.setBeforeStop([&image_downloader, &serial_port_manager, &osc_manager] ()
    {
        serial_port_manager.stop();
        image_downloader.stop();
        osc_manager.close();
    });

    // bound callback
    window.setOnSendButtonClickedFunc(
        [&serial_port_manager, &window] ()
        {
            // 获取SSID和密码
            auto ssid = window.getSSID();
            auto password = window.getPassword();

            // 输入验证
            if (ssid == "请输入WIFI名字（仅支持2.4ghz）" || ssid.empty()) {
                QMessageBox::warning(&window, "输入错误", "请输入有效的WIFI名字");
                return;
            }

            if (password == "请输入WIFI密码" || password.empty()) {
                QMessageBox::warning(&window, "输入错误", "请输入有效的密码");
                return;
            }

            // 构建并发送数据包
            std::string packet = "A2SSID" + ssid + "PWD" + password + "B2";
            serial_port_manager.write_data(packet);

            // 记录操作
            LOG_INFO("已发送WiFi配置: SSID=" + ssid + ", PWD=" + password);

            LOG_INFO("一秒后开始自动重启ESP32...");
            //延时
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    );
    window.setSendBrightnessValueFunc([&serial_port_manager] (int value)
    {
        // 发送亮度控制命令 - 确保亮度值为三位数字
        std::string brightness_str = std::to_string(value);
        // 补齐三位数字，前面加0
        while (brightness_str.length() < 3) {
            brightness_str = std::string("0") + brightness_str;
        }
        std::string packet = "A6" + brightness_str + "B6";
        serial_port_manager.write_data(packet);
        // 记录操作
        LOG_INFO("已设置亮度: " + std::to_string(value));
    });
    window.setOnRestartButtonClickedFunc([&serial_port_manager, &window] ()
    {
        restart_esp32(serial_port_manager, window);
    });
    window.setOnFlashButtonClickedFunc([&serial_port_manager, &window] ()
    {
        flash_esp32(window, serial_port_manager);
    });
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

    std::string camera_ip;

    LOG_INFO("初始化串口");
    // init serial port manager
    serial_port_manager.setDeviceStatusCallback([&window, &camera_ip, &image_downloader, &config]
                                                        (const std::string& ip, int brightness, int power, int version) {
        // 使用Qt的线程安全方式更新UI
        QMetaObject::invokeMethod(&window, [&window, ip, brightness, power, version, &camera_ip, &image_downloader, &config]() {
            // 只在 IP 地址变化时更新显示
            if (camera_ip != ip)
            {
                camera_ip = ip;
                // 更新IP地址显示，添加 http:// 前缀
                window.setIPText("http://" + ip);
                LOG_INFO("IP地址已更新: http://" + ip);
                start_image_download(window, image_downloader, "http://" + camera_ip);
            }
            // 可以添加其他状态更新的日志，如果需要的话
        }, Qt::QueuedConnection);
    });
    serial_port_manager.init();

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

    LOG_INFO("尝试连接串口");
    serial_port_manager.start();

    LOG_INFO("等待串口状态响应");
    while (serial_port_manager.status() == SerialStatus::CLOSED) {}
    LOG_INFO("串口状态响应完毕");

    if (serial_port_manager.status() == SerialStatus::FAILED)
    {
        window.setSerialStatusLabel("串口连接失败");
        LOG_WARN("串口未连接，尝试从配置文件中读取地址...");

        // auto ip = wifi_cache_file_writer.try_get_wifi_config();
        config = config_writer.get_config<PaperTrackerConfig>();
        if (!config.wifi_ip.empty())
        {
            LOG_INFO("从配置文件中读取地址成功");
            camera_ip = config.wifi_ip;
            window.set_config(config);
            auto esp32_future = std::async(std::launch::async, [&window, &image_downloader, camera_ip]()
            {
                start_image_download(window, image_downloader, camera_ip);
            });
        } else
        {
            QMessageBox msgBox;
            msgBox.setWindowIcon(icon);
            msgBox.setText("未找到配置文件信息，请将面捕通过数据线连接到电脑进行首次配置");
            msgBox.exec();
        }
    } else
    {
        LOG_INFO("串口连接成功");
        window.setSerialStatusLabel("串口连接成功");
    }

    LOG_INFO("正在启动视频处理线程...");
    window.set_update_thread([ &window, &image_downloader, &inference, &osc_manager] ()
    {
        update_ui(window, image_downloader, inference, osc_manager);
    });
    window.set_inference_thread([ &window, &image_downloader, &inference, &osc_manager] ()
    {
        inference_image(window, image_downloader, inference, osc_manager);
    });

    int status = QApplication::exec();
    window.stop();

    return status;
}
