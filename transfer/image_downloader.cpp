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
#include "image_downloader.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <QUrl>
#include <QMutexLocker>
#include <QTimer>

ESP32VideoStream::ESP32VideoStream(QObject *parent)
    : QObject(parent), isRunning(false), webSocket(nullptr), heartbeatTimer(nullptr)
{
}

ESP32VideoStream::~ESP32VideoStream()
{
    if (isRunning)
    {
        stop();
    }
}

bool ESP32VideoStream::init(const std::string& url)
{
    if (url.empty())
    {
        return !currentStreamUrl.empty();
    }
    // 保存URL，但不要立即连接
    currentStreamUrl = url;

    // 检查URL是否为WebSocket URL (ws:// 或 wss://)
    if (url.substr(0, 5) != "ws://" && url.substr(0, 6) != "wss://") {
        LOG_INFO("转换HTTP URL为WebSocket URL: " + url);
        // 如果是HTTP URL，转换为WebSocket URL
        if (url.substr(0, 7) == "http://") {
            currentStreamUrl = "ws://" + url.substr(7) + "/ws";
        } else if (url.substr(0, 8) == "https://") {
            currentStreamUrl = "wss://" + url.substr(8) + "/ws";
        } else {
            // 不是标准URL，假设是主机地址，添加ws://
            // 注意：根据HTML测试页面，我们使用"ws://IP:81/ws"格式
            currentStreamUrl = "ws://" + url;
            // 确保URL有正确的端口和路径
            if (currentStreamUrl.find(':') == std::string::npos) {
                // 如果没有端口，添加默认端口81
                currentStreamUrl += ":80";
            }
            if (currentStreamUrl.find("/ws") == std::string::npos) {
                // 如果没有/ws路径，添加它
                currentStreamUrl += "/ws";
            }
        }
    }

    LOG_INFO("初始化WebSocket视频流，URL: " + currentStreamUrl);
    return true;
}

void ESP32VideoStream::checkHeartBeat()
{
    if (image_not_receive_count++ > 50)
    {
        image_not_receive_count = 0;
        isRunning = false;
        stop();
        start();
    }
}

bool ESP32VideoStream::start()
{
    webSocket = new QWebSocket();
    // 禁用代理设置，避免代理相关错误
    webSocket->setProxy(QNetworkProxy::NoProxy);

    connect(webSocket, &QWebSocket::connected,
        this, &ESP32VideoStream::onConnected);
    connect(webSocket, &QWebSocket::disconnected,
            this, &ESP32VideoStream::onDisconnected);
    connect(webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, &ESP32VideoStream::onError);
    connect(webSocket, &QWebSocket::binaryMessageReceived,
            this, &ESP32VideoStream::onBinaryMessageReceived);

    if (!heartbeatTimer)
    {
        heartbeatTimer = new QTimer();
        connect(heartbeatTimer, &QTimer::timeout, this, &ESP32VideoStream::checkHeartBeat);
    }


    if (isRunning) {
        LOG_WARN("视频流已经在运行中");
        return false;
    }

    // 连接WebSocket
    LOG_INFO("开始连接WebSocket: " + currentStreamUrl);
    webSocket->open(QUrl(QString::fromStdString(currentStreamUrl)));

    if (!heartbeatTimer->isActive())
    {
        heartbeatTimer->start(50);
    }
    return true;
}

void ESP32VideoStream::stop()
{
    LOG_INFO("停止WebSocket视频流");
    isRunning = false;

    // 关闭WebSocket
    if (webSocket)
    {
        if (webSocket->state() != QAbstractSocket::UnconnectedState)
        {
            QMetaObject::invokeMethod(webSocket, "close", Qt::QueuedConnection);
        }
        webSocket->deleteLater();
    }

    // 清空图像队列
    QMutexLocker locker(&mutex);
    while (!image_buffer_queue.empty()) {
        image_buffer_queue.pop();
    }
}

cv::Mat ESP32VideoStream::getLatestFrame() const
{
    QMutexLocker locker(&mutex);
    if (image_buffer_queue.empty()) {
        return {};
    }
    return image_buffer_queue.front().clone();
}

void ESP32VideoStream::onConnected()
{
    LOG_INFO("WebSocket连接成功: " + currentStreamUrl);
    isRunning = true;
}

void ESP32VideoStream::onDisconnected()
{
    LOG_DEBUG("WebSocket连接已关闭");
    isRunning = false;
}

void ESP32VideoStream::onError(QAbstractSocket::SocketError error)
{
    QString errorString = webSocket->errorString();
    int errorCode = static_cast<int>(error);
    LOG_DEBUG("WebSocket错误: " + std::to_string(errorCode) +
              " - " + errorString.toStdString() +
              " (URL: " + currentStreamUrl + ")");
    LOG_ERROR("无线连接失败，请确保面捕已经开机且连接上WIFI");
    // 输出更多连接信息
    LOG_DEBUG("连接状态: " + std::to_string(static_cast<int>(webSocket->state())));
    LOG_DEBUG("代理类型: " + std::to_string(static_cast<int>(webSocket->proxy().type())));

    // 如果是代理错误，尝试重新连接一次，使用不同的URL格式
    if (error == QAbstractSocket::ProxyProtocolError) {
        LOG_INFO("检测到代理错误，尝试使用不同的URL格式重新连接...");

        // 尝试构建一个不同格式的URL
        std::string newUrl = currentStreamUrl;
        // 如果URL末尾有 "/ws"，尝试移除它
        if (newUrl.size() > 3 && newUrl.substr(newUrl.size() - 3) == "/ws") {
            newUrl = newUrl.substr(0, newUrl.size() - 3);
            LOG_INFO("尝试新URL: " + newUrl);

            // 重置WebSocket并尝试新URL
            webSocket->close();
            webSocket->setProxy(QNetworkProxy::NoProxy);
            currentStreamUrl = newUrl;
            webSocket->open(QUrl(QString::fromStdString(newUrl)));
            return;
        }
    }

    isRunning = false;
}

void ESP32VideoStream::onBinaryMessageReceived(const QByteArray &message)
{
    isRunning = true;
    image_not_receive_count = 0;
    try {
        // 打印接收到的数据长度以进行调试
        // LOG_DEBUG("接收到WebSocket数据: " + std::to_string(message.size()) + " 字节");

        // 检查数据是否足够长
        if (message.size() < 10) {
            LOG_WARN("接收到的数据太短，不可能是有效的图像");
            return;
        }

        // 直接使用OpenCV解码数据 - 模仿HTML测试页面的处理方式
        std::vector<uchar> buffer(message.begin(), message.end());
        cv::Mat rawFrame = cv::imdecode(buffer, cv::IMREAD_COLOR);

        if (!rawFrame.empty()) {
            LOG_DEBUG("成功解码图像，尺寸: " + std::to_string(rawFrame.cols) + "x" + std::to_string(rawFrame.rows));

            QMutexLocker locker(&mutex);
            if (image_buffer_queue.empty()) {
                image_buffer_queue.push(std::move(rawFrame));
            } else {
                image_buffer_queue.pop();
                image_buffer_queue.push(std::move(rawFrame));
            }
        } else {
            // 如果OpenCV解码失败，尝试Qt的方法
            QImage image;
            if (image.loadFromData(message, "JPEG")) {
                LOG_DEBUG("Qt成功解码JPEG图像");
                cv::Mat frame = QImageToCvMat(image);

                if (!frame.empty()) {
                    QMutexLocker locker(&mutex);
                    image_not_receive_count = 0;
                    if (image_buffer_queue.empty()) {
                        image_buffer_queue.push(std::move(frame));
                    } else {
                        image_buffer_queue.pop();
                        image_buffer_queue.push(std::move(frame));
                    }
                }
            } else {
                // 如果Qt也失败，记录数据头部信息
                LOG_WARN("无法解码接收到的图像数据");
                //LOG_DEBUG("数据前16字节: " + bytesToHexString(message.left(16)));

                // 尝试检测数据类型
                if (message.size() > 4) {
                    uint32_t magic = 0;
                    memcpy(&magic, message.data(), 4);
                    LOG_DEBUG("数据前4字节魔数: 0x" + QString::number(magic, 16).toStdString());
                }
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("处理WebSocket消息时出错: " + std::string(e.what()));
    }
}

cv::Mat ESP32VideoStream::QImageToCvMat(const QImage &image) const
{
    switch (image.format()) {
    case QImage::Format_RGB888: {
        cv::Mat mat(image.height(), image.width(), CV_8UC3,
                   const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::Mat result;
        cv::cvtColor(mat, result, cv::COLOR_RGB2BGR);
        return result;
    }
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied: {
        cv::Mat mat(image.height(), image.width(), CV_8UC4,
                   const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::Mat result;
        cv::cvtColor(mat, result, cv::COLOR_RGBA2BGR);
        return result;
    }
    default: {
        // 对于其他格式，转换为RGB888再处理
        QImage converted = image.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat(converted.height(), converted.width(), CV_8UC3,
                   const_cast<uchar*>(converted.bits()), converted.bytesPerLine());
        cv::Mat result;
        cv::cvtColor(mat, result, cv::COLOR_RGB2BGR);
        return result;
    }
    }
}