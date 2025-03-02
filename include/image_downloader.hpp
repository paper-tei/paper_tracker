//
// Created by JellyfishKnight on 25-3-2.
//

#ifndef IMAGE_DOWNLOADER_HPP
#define IMAGE_DOWNLOADER_HPP
#include <string>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include <opencv2/core.hpp>

#include "thread_pool.hpp"

const std::string BOUNDARY = "123456789000000000000987654321";
const std::string BOUNDARY_HEADER = "\r\n\r\n";
const std::string BOUNDARY_END = "--123456789000000000000987654321";

class ImageDownloader {
public:
    ImageDownloader(const std::string& ip, const std::string& port);

    ~ImageDownloader();

    void start_video_from_esp32(const std::string& url, std::vector<char>& buffer);

    // underwear function. Author: paper-tei
    bool start_video_stream_server();
    // underwear function. Author: paper-tei
    void stop_video_stream_server();
private:
    cv::Mat get_video_frame();

    void update_video_frame(cv::Mat frame);

    size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);

    // underwear function. Author: paper-tei
    void client_handler(SOCKET client_socket);

    SOCKET listenSocket;
    std::string ip;
    std::string port;
    utils::ThreadPool thread_pool_;

    bool server_running = false;

    std::queue<cv::Mat> video_frame_queue_;

    std::mutex bufferMutex;
    std::vector<char> imageBuffer;
};




#endif //IMAGE_DOWNLOADER_HPP
