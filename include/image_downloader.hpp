// //
// // Created by JellyfishKnight on 25-3-2.
// //
//
// #ifndef IMAGE_DOWNLOADER_HPP
// #define IMAGE_DOWNLOADER_HPP
// #include <string>
// #include <chrono>
// #include <thread>
// #include <curl/curl.h>
//
//
// class ImageDownloader {
// public:
//     ImageDownloader() = default;
//     ~ImageDownloader() = default;
//
//     void download_image(const std::string& url, const std::string& save_path);
//
//     bool start_video_stream_server(int port = 8080);
//
//     void stop_video_stream_server();
// private:
//     void write_callback(void* contents, size_t size, size_t nmemb, void* userp);
//
// };
//
//
//
//
// #endif //IMAGE_DOWNLOADER_HPP
