#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <QLabel>
#include <opencv2/core.hpp>
#include "logger.hpp"

// 前向声明
typedef void CURL;

class ESP32VideoStream {
public:
    // 回调函数类型，用于接收新的视频帧
    using FrameCallback = std::function<void(const cv::Mat&)>;

    // 构造函数和析构函数
    ESP32VideoStream(QLabel *state_label);
    ~ESP32VideoStream();

    // 初始化视频流，设置ESP32的URL和可选的回调函数
    bool init(const std::string& url, FrameCallback callback = nullptr);

    // 开始接收视频流
    bool start();

    // 停止视频流
    void stop();

    // 获取最新的帧
    cv::Mat getLatestFrame();

    // 检查流是否正在运行
    bool isStreaming() const { return isRunning; }

private:
    // 缓冲区最大大小，防止无限增长
    static constexpr size_t MAX_BUFFER_SIZE = 10 * 1024 * 1024;  // 10MB

    // CURL写回调函数 - 处理数据块
    static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata);

    // 处理流数据
    size_t handleStreamData(char* data, size_t dataSize);

    // 处理缓冲区中的所有可能完整的帧
    void processFramesInBuffer();

    // 寻找下一个边界位置
    size_t findNextBoundary(size_t startPos);

    // 寻找双换行符位置
    size_t findDoubleNewline(size_t startPos);

    // 解析Content-Length头
    int parseContentLength(size_t headerStart, size_t headerEnd);

    // 处理JPEG帧
    void processJpegFrame(const std::vector<char>& jpegData);

    // 视频流处理线程
    void streamThreadFunc();

    // 成员变量
    std::atomic<bool> isRunning;
    std::thread streamThread;
    std::vector<char> streamBuffer;
    cv::Mat latestFrame;
    FrameCallback frameCallback;
    std::string currentStreamUrl;
    CURL* curl;
    QLabel *state_label;
};
