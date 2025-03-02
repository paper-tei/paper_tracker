//
// Created by JellyfishKnight on 25-3-2.
//

#ifndef IMAGE_DOWNLOADER_HPP
#define IMAGE_DOWNLOADER_HPP
#include <string>
#include <chrono>
#include <vector>
#include <mutex>
#include <opencv2/core.hpp>

#include "thread_pool.hpp"

const std::string BOUNDARY = "123456789000000000000987654321";
const std::string BOUNDARY_HEADER = "\r\n\r\n";
const std::string BOUNDARY_END = "--123456789000000000000987654321";

class ImageDownloader {
public:
    ImageDownloader();

    ~ImageDownloader();

    void start_video_from_esp32(const std::string& url, std::vector<char>* buffer);

    cv::Mat get_video_frame();
private:
    void update_video_frame(cv::Mat frame);

    size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);

    utils::ThreadPool thread_pool_;
    std::queue<cv::Mat> video_frame_queue_;
    std::mutex bufferMutex;
    std::vector<char> imageBuffer;
};




#endif //IMAGE_DOWNLOADER_HPP
