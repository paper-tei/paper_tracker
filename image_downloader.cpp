//
// Created by JellyfishKnight on 25-3-2.
//
#include "image_downloader.hpp"

#include <iostream>


bool ImageDownloader::StartVideoStreamServer(int port) {

}


void ImageDownloader::StopVideoStreamServer() {

}

void ImageDownloader::DownloadImage(const std::string& url, const std::string& save_path) {
    int maxRetries = 30;  // 最大重试次数
    int retryCount = 0;  // 当前重试次数
    CURL* curl = nullptr;
    CURLcode res;

    if (retryCount < maxRetries) {
        curl = curl_easy_init();
        if (curl) {
            // 设置 URL
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            // 设置写回调函数
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            // **低速检测**：
            curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 1L);
            curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 3000L);




            // 执行请求
            res = curl_easy_perform(curl);

            if (res == CURLE_OK) {
                std::cout << "CURL request successful" << std::endl;
                curl_easy_cleanup(curl);
                return;  // 请求成功，返回
            }
            else {
                std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
                // 如果请求失败，增加重试次数
                retryCount++;
                std::cout << "Retrying (" << retryCount << "/" << maxRetries << ")..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待 1 秒后重试
            }

            // 清理资源
            curl_easy_cleanup(curl);
        }
        else {
            std::cerr << "Failed to initialize CURL." << std::endl;
        }
    }

    if (retryCount == maxRetries) {
        std::cerr << "Failed to download image after " << maxRetries << " retries." << std::endl;
    }

}


