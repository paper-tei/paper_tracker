//
// Created by JellyfishKnight on 25-2-28.
//
#include "video_reader.hpp"
#include "opencv2/videoio.hpp"

VideoReader::VideoReader(const std::string &video_path) : video_path(video_path){
}

void VideoReader::open_video(const std::string &video_path) {
    if (video_path.empty() && this->video_path.empty()) {
        minilog::log_error(nullptr, "Video path is empty");
        return;
    }
    cap.open(0, cv::CAP_DSHOW);

    this->video_path = video_path;
}

bool VideoReader::is_opened() const
{
    return cap.isOpened();
}

cv::Mat VideoReader::get_image() {
    if (!cap.isOpened()) {
        minilog::log_error(nullptr, "Video is not opened");
        return cv::Mat();
    }
    cap >> frame;
    if (frame.empty())
    {
        minilog::log_warn(nullptr, "Video has ended");
    }
    return frame;
}

