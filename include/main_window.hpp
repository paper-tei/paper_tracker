//
// Created by JellyfishKnight on 2025/2/25.
//

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <future>
#include <inference.hpp>
#include <thread>

#include "ui_main_window.h"
#include "video_reader.hpp"

class PaperTrackMainWindow final : public QWidget {
public:
    explicit PaperTrackMainWindow(QWidget *parent = nullptr) :
        QWidget(parent) {
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

        show_video_thread = std::thread([this]() {
            cv::Mat frame;
            while (true) {
                if (!video_reader.is_opened())
                {
                    continue;
                }
                frame = std::move(video_reader.get_image());
                if (frame.empty())
                {
                    break;
                }
                auto infer_frame = frame.clone();
                inference.inference(infer_frame);
                cv::resize(frame, frame, cv::Size(361, 251));
                // show frame on the label
                cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
                QImage qimage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
                ui.ImageLabel->setPixmap(QPixmap::fromImage(qimage));
                ui.ImageLabel->setScaledContents(true);
                ui.ImageLabel->update();
                cv::waitKey(20);
            }
        });
    }

    ~PaperTrackMainWindow()
    {
        show_video_thread.join();
    }

private:
    std::thread show_video_thread;

    void bound_pages();

    VideoReader video_reader;
    Inference inference;

    Ui_PaperTrackerMainWindow ui{};
};


#endif //MAIN_WINDOW_HPP
