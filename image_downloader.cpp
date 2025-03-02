//
// Created by JellyfishKnight on 25-3-2.
//
#include "image_downloader.hpp"

#include <iostream>
#include <curl/curl.h>
#include <opencv2/imgcodecs.hpp>


ImageDownloader::ImageDownloader() : thread_pool_(4) {}

ImageDownloader::~ImageDownloader()
{
    thread_pool_.stop_now();
}

cv::Mat ImageDownloader::get_video_frame()
{
    if (video_frame_queue_.empty())
    {
        return {};
    }
    auto frame = std::move(video_frame_queue_.front());
    video_frame_queue_.pop();
    return frame;
}

void ImageDownloader::start_video_from_esp32(const std::string& url, std::vector<char>* buffer)
{
    int maxRetries = 30;
    int retryCount = 0;
    CURL* curl = nullptr;

    if (retryCount < maxRetries) {
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ImageDownloader::write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*>(buffer));
            curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 1L);
            curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 3000L);

            const auto res = curl_easy_perform(curl);

            if (res == CURLE_OK) {
                std::cout << "CURL request successful" << std::endl;
                curl_easy_cleanup(curl);
                return;
            } else {
                std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
                retryCount++;
                std::cout << "Retrying (" << retryCount << "/" << maxRetries << ")..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));  // ??? 1 ???????
            }
            curl_easy_cleanup(curl);
        }
        else {
            std::cerr << "Failed to initialize CURL." << std::endl;
        }
    }

    if (retryCount >= maxRetries) {
        std::cerr << "Failed to download image after " << maxRetries << " retries." << std::endl;
    }
}

void ImageDownloader::update_video_frame(cv::Mat frame)
{
    video_frame_queue_.push(std::move(frame));
}

size_t ImageDownloader::write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    static std::vector<char> residualBuffer;
    const size_t totalSize = size * nmemb;
    std::vector<char>* buffer = static_cast<std::vector<char>*>(userp);
    std::cout << buffer->size();

    if (!residualBuffer.empty()) {
        buffer->insert(buffer->begin(), residualBuffer.begin(), residualBuffer.end());
        residualBuffer.clear();
    }

    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer->insert(buffer->end(),
            static_cast<char*>(contents),
            static_cast<char*>(contents) + totalSize);
    }

    while (true) {
        auto boundaryStart = std::search(
            buffer->begin(), buffer->end(),
            BOUNDARY_HEADER.begin(), BOUNDARY_HEADER.end()
        );

        if (boundaryStart == buffer->end()) break;

        auto boundaryEnd = std::search(
            boundaryStart + BOUNDARY_HEADER.size(), buffer->end(),
            BOUNDARY_END.begin(), BOUNDARY_END.end()
        );

        if (boundaryEnd == buffer->end()) {
            residualBuffer.assign(boundaryStart, buffer->end());
            buffer->clear();
            break;
        }

        auto payloadStart = std::search(
            boundaryStart, boundaryEnd,
            "\r\n\r\n", "\r\n\r\n" + 4
        );

        if (payloadStart != boundaryEnd) {
            payloadStart += 4;
            std::vector<char> frameData(payloadStart, boundaryEnd);
            {
                std::lock_guard<std::mutex> lock(bufferMutex);
                imageBuffer.swap(frameData);
            }
            if (imageBuffer.size() > 5 * 1024 * 1024) {
                std::lock_guard<std::mutex> lock(bufferMutex);
                imageBuffer.clear();
            }
            // convert frame data(gray) to cv::Mat
            cv::Mat frame;
            frame = cv::imdecode(imageBuffer, cv::IMREAD_COLOR);
            update_video_frame(frame);
        }

        // ?????????
        buffer->erase(buffer->begin(), boundaryEnd + BOUNDARY_END.size());
    }

    return totalSize;

}






