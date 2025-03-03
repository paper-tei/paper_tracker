#include "image_downloader.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <curl/curl.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

// 互斥锁用于保护帧数据
std::mutex frameMutex;

// 定义MJPEG流的边界标记
const std::string CONTENT_TYPE_HEADER = "Content-Type:";
const std::string BOUNDARY_MARKER = "boundary=";
const std::string CONTENT_LENGTH_HEADER = "Content-Length:";
const std::string DOUBLE_NEWLINE = "\r\n\r\n";

ESP32VideoStream::ESP32VideoStream() 
    : isRunning(false), 
      frameCallback(nullptr),
      currentStreamUrl(""),
      curl(nullptr) {
}

ESP32VideoStream::~ESP32VideoStream() {
    stop();
}

// 初始化视频流，设置ESP32的URL和可选的回调函数
bool ESP32VideoStream::init(const std::string& url, FrameCallback callback) {
    currentStreamUrl = url;
    frameCallback = callback;
    
    // 初始化CURL
    curl_global_init(CURL_GLOBAL_ALL);
    return true;
}

// 开始接收视频流
bool ESP32VideoStream::start() {
    if (isRunning) {
        std::cerr << "视频流已经在运行!" << std::endl;
        return false;
    }
    
    isRunning = true;
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "初始化curl失败" << std::endl;
        isRunning = false;
        return false;
    }

    // 设置CURL选项
    curl_easy_setopt(curl, CURLOPT_URL, currentStreamUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L); // 无超时
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L); // 连接超时10秒

    // 低速检测（防止连接丢失时卡住）
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 10L);

    std::cout << "开始连接ESP32视频流: " << currentStreamUrl << std::endl;

    // 创建线程处理视频流
    streamThread = std::thread(&ESP32VideoStream::streamThreadFunc, this);
    return true;
}

// 停止视频流
void ESP32VideoStream::stop() {
    if (!isRunning) {
        return;
    }
    isRunning = false;
    // 清理
    curl_easy_cleanup(curl);
    curl = nullptr;
    if (streamThread.joinable()) {
        streamThread.join();
    }
    
    // 清理curl
    curl_global_cleanup();
}

// 获取最新的帧
cv::Mat ESP32VideoStream::getLatestFrame() {
    std::lock_guard<std::mutex> lock(frameMutex);
    return latestFrame.clone();
}

// CURL写回调函数 - 处理数据块
size_t ESP32VideoStream::writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    ESP32VideoStream* stream = static_cast<ESP32VideoStream*>(userdata);
    return stream->handleStreamData(ptr, size * nmemb);
}

// 处理流数据
size_t ESP32VideoStream::handleStreamData(char* data, size_t dataSize) {
    // 添加数据到缓冲区
    streamBuffer.insert(streamBuffer.end(), data, data + dataSize);
    
    // 处理缓冲区中的所有可能完整的帧
    processFramesInBuffer();
    
    return dataSize;
}

// 处理缓冲区中的所有可能完整的帧
void ESP32VideoStream::processFramesInBuffer() {
    // 查找并处理所有完整的帧
    size_t searchPos = 0;
    
    while (searchPos < streamBuffer.size()) {
        // 寻找帧边界
        size_t boundaryPos = findNextBoundary(searchPos);
        if (boundaryPos == std::string::npos) {
            break;
        }
        
        // 寻找双换行符（表示头部结束，内容开始）
        size_t headerEnd = findDoubleNewline(boundaryPos);
        if (headerEnd == std::string::npos) {
            searchPos = boundaryPos + 1;
            continue;
        }
        
        // 寻找内容长度
        int contentLength = parseContentLength(boundaryPos, headerEnd);
        
        // 计算内容开始位置
        size_t contentStart = headerEnd + DOUBLE_NEWLINE.length();
        
        // 检查是否有足够的数据
        if (contentLength > 0 && contentStart + contentLength <= streamBuffer.size()) {
            // 提取JPEG数据
            std::vector<char> jpegData(streamBuffer.begin() + contentStart, 
                                      streamBuffer.begin() + contentStart + contentLength);
            
            // 处理JPEG帧
            processJpegFrame(jpegData);
            // 移动到下一个搜索位置
            searchPos = contentStart + contentLength;

            try {
                std::vector<char> newBuffer(streamBuffer.begin() + searchPos, streamBuffer.end());
                streamBuffer.swap(newBuffer); // 交换比直接擦除更安全
            } catch (const std::exception& e) {
                std::cerr << "清理缓冲区时出错: " << e.what() << std::endl;
                streamBuffer.clear(); // 如果失败则清空
            }
        } else {
            // 没有足够的数据，等待更多
            searchPos = boundaryPos + 1;
        }
    }




    // 防止缓冲区无限增长
    if (streamBuffer.size() > MAX_BUFFER_SIZE) {
        std::cerr << "警告: 缓冲区过大，可能丢失帧" << std::endl;
        streamBuffer.clear();
    }
}

// 寻找下一个边界位置
size_t ESP32VideoStream::findNextBoundary(size_t startPos) {
    // 这里简化了边界检测，实际可能需要动态确定边界字符串
    const std::string boundaryStart = "--";
    
    return std::search(streamBuffer.begin() + startPos, streamBuffer.end(), 
                      boundaryStart.begin(), boundaryStart.end()) - streamBuffer.begin();
}

// 寻找双换行符位置
size_t ESP32VideoStream::findDoubleNewline(size_t startPos) {
    auto pos = std::search(streamBuffer.begin() + startPos, streamBuffer.end(), 
                           DOUBLE_NEWLINE.begin(), DOUBLE_NEWLINE.end()) - streamBuffer.begin();
    
    if (pos >= streamBuffer.size()) {
        return std::string::npos;
    }
    
    return pos;
}

// 解析Content-Length头
int ESP32VideoStream::parseContentLength(size_t headerStart, size_t headerEnd) {
    std::string header(streamBuffer.begin() + headerStart, streamBuffer.begin() + headerEnd);
    
    // 寻找Content-Length
    size_t contentLengthPos = header.find(CONTENT_LENGTH_HEADER);
    if (contentLengthPos != std::string::npos) {
        // 获取冒号之后的位置
        size_t colonPos = header.find(':', contentLengthPos);
        if (colonPos != std::string::npos) {
            // 查找行尾
            size_t eolPos = header.find("\r\n", colonPos);
            if (eolPos != std::string::npos) {
                // 提取长度值
                std::string lengthStr = header.substr(colonPos + 1, eolPos - colonPos - 1);
                // 去除前导空格
                lengthStr.erase(0, lengthStr.find_first_not_of(" \t"));
                // 转换为整数
                try {
                    return std::stoi(lengthStr);
                } catch (...) {
                    return -1;
                }
            }
        }
    }
    
    return -1;  // 未找到长度
}

// 处理JPEG帧
void ESP32VideoStream::processJpegFrame(const std::vector<char>& jpegData) {
    // 使用OpenCV解码JPEG数据
    try {
        cv::Mat frame = cv::imdecode(cv::Mat(jpegData), cv::IMREAD_COLOR);
        
        if (!frame.empty()) {
            // 更新最新帧
            {
                std::lock_guard<std::mutex> lock(frameMutex);
                latestFrame = frame.clone();
            }
            
            // 如果有回调函数，则调用
            if (frameCallback) {
                frameCallback(frame);
            }
        }
    } catch (const cv::Exception& e) {
        std::cerr << "处理JPEG帧出错: " << e.what() << std::endl;
    }
}

// 视频流处理线程
void ESP32VideoStream::streamThreadFunc() {
    // 启动连接
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() 失败: " << curl_easy_strerror(res) << std::endl;
    }
    std::cout << "ESP32视频流线程退出" << std::endl;
    isRunning = false;
}