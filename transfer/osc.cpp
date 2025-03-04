//
// Created by JellyfishKnight on 25-3-1.
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
#include "osc.hpp"

// osc_manager.cpp
#include "osc.hpp"
#include <cmath>
#include <iostream>
// 引入oscpack库
#include "../3rdParty/oscpack/osc/OscOutboundPacketStream.h"
#include "../3rdParty/oscpack/ip/UdpSocket.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

#include "osc.hpp"
#include <cmath>
#include <iostream>

// 引入oscpack库
#include "../3rdParty/oscpack/osc/OscOutboundPacketStream.h"
#include "../3rdParty/oscpack/ip/UdpSocket.h"

#define OUTPUT_BUFFER_SIZE 1024

OscManager::OscManager()
    : address_("127.0.0.1"),
      port_(8888),
      location_prefix_(""),
      multiplier_(1.0f),
      socket_(nullptr)
{
}

OscManager::~OscManager() {
    close();
}

bool OscManager::init(const std::string& address, int port) {
    address_ = address;
    port_ = port;

    try {
        socket_ = std::make_unique<UdpTransmitSocket>(
            IpEndpointName(address_.c_str(), port_)
        );
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("OSC初始化错误: " + QString(e.what()));
        return false;
    }
}

void OscManager::setLocationPrefix(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(mutex_);
    location_prefix_ = prefix;
}

void OscManager::setMultiplier(float multiplier) {
    std::lock_guard<std::mutex> lock(mutex_);
    multiplier_ = multiplier;
}

bool OscManager::sendModelOutput(const std::vector<float>& output) {
    if (!socket_) {
        LOG_ERROR("OSC socket未初始化");
        return false;
    }

    // 确保有足够的输出值
    if (output.size() < 45) {
        LOG_ERROR("模型输出数组大小不足，需要45个元素，实际: " + QString::number(output.size()));
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    try {
        // 计算最大裁剪值
        float max_clip_value = std::pow(10, std::floor(std::log10(multiplier_)));

        // 定义所有ARKit模型输出名称
        const std::string blend_shapes[] = {
            "/cheekPuffLeft", "/cheekPuffRight",
            "/cheekSuckLeft", "/cheekSuckRight",
            "/jawOpen", "/jawForward", "/jawLeft", "/jawRight",
            "/noseSneerLeft", "/noseSneerRight",
            "/mouthFunnel", "/mouthPucker",
            "/mouthLeft", "/mouthRight",
            "/mouthRollUpper", "/mouthRollLower",
            "/mouthShrugUpper", "/mouthShrugLower",
            "/mouthClose",
            "/mouthSmileLeft", "/mouthSmileRight",
            "/mouthFrownLeft", "/mouthFrownRight",
            "/mouthDimpleLeft", "/mouthDimpleRight",
            "/mouthUpperUpLeft", "/mouthUpperUpRight",
            "/mouthLowerDownLeft", "/mouthLowerDownRight",
            "/mouthPressLeft", "/mouthPressRight",
            "/mouthStretchLeft", "/mouthStretchRight",
            "/tongueOut", "/tongueUp", "/tongueDown",
            "/tongueLeft", "/tongueRight",
            "/tongueRoll", "/tongueBendDown",
            "/tongueCurlUp", "/tongueSquish",
            "/tongueFlat", "/tongueTwistLeft",
            "/tongueTwistRight"
        };

        // 循环发送每个输出值
        for (size_t i = 0; i < output.size() && i < sizeof(blend_shapes)/sizeof(blend_shapes[0]); ++i) {
            // 应用乘数并限制范围
            float value = std::min(output[i] * multiplier_, max_clip_value);

            // 构建完整地址
            std::string address = location_prefix_ + blend_shapes[i];

            // 创建OSC消息
            char buffer[OUTPUT_BUFFER_SIZE];
            osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

            p << osc::BeginMessage(address.c_str())
              << value
              << osc::EndMessage;

            // 发送OSC消息
            socket_->Send(p.Data(), p.Size());
        }

        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("发送OSC消息错误: " + QString(e.what()));
        return false;
    }
}

void OscManager::close() {
    socket_.reset();
}


