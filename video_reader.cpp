//
// Created by JellyfishKnight on 25-2-28.
//
#include "video_reader.hpp"
#include "opencv2/videoio.hpp"
#include <QPainter>

VideoReader::VideoReader(const std::string &video_path) : video_path(video_path){}

void VideoReader::open_video(const std::string &video_path) {
    if (video_path.empty() && this->video_path.empty()) {
        LOG_ERROR("视频路径为空");
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
        LOG_ERROR("视频打开失败");
        return cv::Mat();
    }
    cap >> frame;
    if (frame.empty())
    {
        LOG_WARN("视频播放结束");
    }
    return frame;
}