//
// Created by JellyfishKnight on 25-2-28.
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
#include "video_reader.hpp"
#include "../3rdParty/opencv-mvsc/opencv/build/include/opencv2/videoio.hpp"
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