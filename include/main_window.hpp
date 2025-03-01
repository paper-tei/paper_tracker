//
// Created by JellyfishKnight on 2025/2/25.
//

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <future>
#include <inference.hpp>
#include <thread>

#include "osc.hpp"
#include "ui_main_window.h"
#include "video_reader.hpp"
#include "serial.hpp"
#include "roi_event.hpp"

class PaperTrackMainWindow final : public QWidget {
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr) :
        QWidget(parent),serial_port_manager_(ui.LogText) {
        setFixedSize(848, 538);
        ui.setupUi(this);
        ui.LogText->setMaximumBlockCount(200);
        bound_pages();
        auto fu = std::async(std::launch::async, [this]()
        {
            video_reader.open_video("D:/Babble/test_video.mkv");
            if (!video_reader.is_opened())
            {
                minilog::log_error(ui.LogText, "Video is not opened");
                exit(1);
            }
        });

        inference.load_model("./model/model.onnx");
        osc_manager_.init("127.0.0.1", 8888);  // 使用默认参数
        osc_manager_.setLocationPrefix("");    // 空前缀
        osc_manager_.setMultiplier(1.0f);      // 默认乘数

        ROIEventFilter *roiFilter = new ROIEventFilter([this] (QRect rect)
        {
            roi_rect = cv::Rect(rect.x(), rect.y(), rect.width(), rect.height());
        },ui.ImageLabel);
        ui.ImageLabel->installEventFilter(roiFilter);

        show_video_thread = std::thread([this]() {
            while (!window_closed) {
                if (!video_reader.is_opened())
                {
                    continue;
                }
                cv::Mat frame = std::move(video_reader.get_image());
                if (frame.empty())
                {
                    break;
                }
                cv::resize(frame, frame, cv::Size(361, 251));
                auto infer_frame = frame.clone();
                if (!roi_rect.empty())
                {
                    infer_frame = infer_frame(roi_rect);
                }

                inference.inference(infer_frame);

                // draw rect on frame
                cv::rectangle(frame, roi_rect, cv::Scalar(0, 255, 0), 2);

                std::vector<float> output = inference.get_output();
                if (!output.empty()) {
                    osc_manager_.sendModelOutput(output);
                }

                // show frame on the label
                cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
                QImage qimage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
                ui.ImageLabel->setPixmap(QPixmap::fromImage(qimage));
                ui.ImageLabel->setScaledContents(true);
                ui.ImageLabel->update();
                cv::waitKey(20);
            }
        });

        serial_port_manager_.start();
    }

    ~PaperTrackMainWindow()
    {
        window_closed = true;
        show_video_thread.join();
        serial_port_manager_.stop();
    }

private:
    bool window_closed = false;

    std::thread show_video_thread;

    void bound_pages();

    VideoReader video_reader;
    Inference inference;
    SerialPortManager serial_port_manager_;

    cv::Rect roi_rect;

    // OSC设置相关变量
    OscManager osc_manager_;
    std::string osc_address_ = "127.0.0.1";
    int osc_port_ = 8888;
    std::string osc_prefix_ = "";
    float osc_multiplier_ = 1.0f;
    bool osc_enabled_ = true;

    Ui_PaperTrackerMainWindow ui{};
};


#endif //MAIN_WINDOW_HPP
