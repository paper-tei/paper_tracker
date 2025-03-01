//
// Created by JellyfishKnight on 25-2-28.
//

#ifndef VIDEO_READER_HPP
#define VIDEO_READER_HPP

#include <QLabel>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include "logger.h"


class VideoReader
{
public:
    VideoReader(const std::string &video_path = {});

    void open_video(const std::string &video_path);

    cv::Mat get_image();

    void set_video_path(const std::string &video_path);

    bool is_opened() const;

private:
    cv::VideoCapture cap;
    cv::Mat frame;
    std::string video_path;
};




#endif //VIDEO_READER_HPP
