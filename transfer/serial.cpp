//
// Created by Paper-tei on 2025/2/26.
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
#pragma once

#include "serial.hpp"
#include <regex>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>    // 包含 GUID_DEVCLASS_PORTS
#include <string>
#include <cstring>      // for strstr()
#include <QMessageBox>
#include <QProcess>
#include <QProgressDialog>
#include <thread>

#define COM_PORT "COM101"  // 指定要打开的串口号



// 修改SerialPortManager的构造函数
SerialPortManager::SerialPortManager(QObject* parent) : QObject(parent), serialPort(new QSerialPort(parent)) {
    m_status = SerialStatus::CLOSED;
}

void SerialPortManager::init()
{
    LOG_INFO("正在搜索ESP32-S3设备...");
    std::string portName = FindEsp32S3Port();
    serialPort = new QSerialPort(nullptr);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    serialPort->setStopBits(QSerialPort::OneStop);
    if (portName.empty()) {
        LOG_WARN("无法找到ESP32-S3设备，尝试使用默认端口COM101");
        serialPort->setPortName(COM_PORT);
    } else {
        // portName = "COM5";
        currentPort = portName;
        LOG_INFO("尝试连接到端口:" + portName);
        serialPort->setPortName(QString::fromStdString(portName));
        // 使用FILE_FLAG_OVERLAPPED打开串口   以启用异步I/O
    }
    readerThread = new QThread;
    reader = new SerialReader(serialPort, &m_status);
    writerThread = new QThread;
    writer = new SerialWriter(serialPort, &m_status);
    // 读取线程实现
    reader->moveToThread(readerThread);
    // connect(serialPort, &QSerialPort::readyRead, reader, &SerialReader::onReadyRead);
    connect(readerThread, &QThread::started, reader, &SerialReader::onReadyRead);  // 确保读取线程开始后读取数据

    // 创建和管理写入线程
    writer->moveToThread(writerThread);
    connect(writerThread, &QThread::started, writer, &SerialWriter::processWriteQueue);
    if (serialPort->open(QIODevice::ReadWrite))
    {
        LOG_INFO("串口打开成功");
        m_status = SerialStatus::OPENED;
    } else
    {
        LOG_ERROR("串口打开失败");
        m_status = SerialStatus::FAILED;
    }
}

SerialPortManager::~SerialPortManager()
{
    if (m_status == SerialStatus::OPENED)
    {
        stop();
    }
}

std::string SerialPortManager::FindEsp32S3Port() {
    if (!currentPort.empty())
    {
        return currentPort;
    }

    std::string targetPort;

    // 设备标识符 - 支持两种可能的ID格式
    const char* targetDeviceIds[] = {
        "USB\\VID_303A&PID_1001&REV_0101&MI_00",
        "USB\\VID_303A&PID_1001&MI_00"
    };

    // 获取设备信息集句柄
    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVCLASS_PORTS,  // 过滤为串口设备类
        nullptr,                  // 不指定枚举器字符串
        nullptr,                  // 父窗口句柄
        DIGCF_PRESENT          // 仅获取当前存在的设备
    );

    if (hDevInfo == INVALID_HANDLE_VALUE) {
        LOG_ERROR("获取设备信息集失败，错误码: " + std::to_string(GetLastError()));
        return "";
    }

    // 枚举设备
    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++) {
        // 获取设备实例ID
        char instanceId[256] = {0};
        if (!SetupDiGetDeviceInstanceIdA(hDevInfo, &devInfoData, instanceId, sizeof(instanceId), nullptr)) {
            continue;
        }
        LOG_INFO("检查设备: " + instanceId);
        // 检查是否是目标设备 - 尝试所有可能的ID
        bool isTargetDevice = false;
        for (const char* targetDeviceId : targetDeviceIds) {
            if (strstr(instanceId, targetDeviceId) != nullptr) {
                isTargetDevice = true;
                LOG_INFO("匹配成功，找到目标设备: " + targetDeviceId);
                break;
            }
        }

        if (isTargetDevice) {
            // 获取串口名称
            HKEY hKey = SetupDiOpenDevRegKey(
                hDevInfo,
                &devInfoData,
                DICS_FLAG_GLOBAL,
                0,
                DIREG_DEV,
                KEY_READ
            );

            if (hKey != INVALID_HANDLE_VALUE) {
                char portName[16] = {0};
                DWORD dwSize = sizeof(portName);
                DWORD dwType = 0;

                if (RegQueryValueExA(hKey, "PortName", nullptr, &dwType, reinterpret_cast<LPBYTE>(portName), &dwSize) == ERROR_SUCCESS) {
                    targetPort = portName;
                    LOG_INFO("找到ESP32-S3设备的COM端口: " + targetPort);
                }

                RegCloseKey(hKey);
                break;
            }
        }
    }

    // 调试：遍历所有可用的COM端口
    SP_DEVINFO_DATA devInfoData2;
    devInfoData2.cbSize = sizeof(SP_DEVINFO_DATA);
    LOG_INFO("系统中所有可用的COM端口: ");
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData2); i++) {
        HKEY hKey = SetupDiOpenDevRegKey(
            hDevInfo,
            &devInfoData2,
            DICS_FLAG_GLOBAL,
            0,
            DIREG_DEV,
            KEY_READ
        );

        if (hKey != INVALID_HANDLE_VALUE) {
            char portName[16] = {0};
            DWORD dwSize = sizeof(portName);
            DWORD dwType = 0;

            if (RegQueryValueExA(hKey, "PortName", nullptr, &dwType, reinterpret_cast<LPBYTE>(portName), &dwSize) == ERROR_SUCCESS) {
                // 获取友好名称
                char friendlyName[256] = {0};
                DWORD nameSize = sizeof(friendlyName);
                if (SetupDiGetDeviceRegistryPropertyA(
                    hDevInfo,
                    &devInfoData2,
                    SPDRP_FRIENDLYNAME,
                    nullptr,
                    reinterpret_cast<PBYTE>(friendlyName),
                    nameSize,
                    nullptr)) {
                    LOG_INFO(" - " + portName + " (" + friendlyName + ")");
                } else {
                    LOG_INFO(" - " + portName);
                }
            }

            RegCloseKey(hKey);
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    if (targetPort.empty()) {
        LOG_ERROR("未找到ESP32-S3设备的COM端口");
    }

    return targetPort;
}

void SerialPortManager::start()
{
    readerThread->start();
    writerThread->start();
    junk_data_thread = std::thread([this]() {
        while (m_status == SerialStatus::OPENED) {
            // 发送垃圾数据以保持串口连接
            write_data("A0B0");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

}

void SerialPortManager::stop()
{
    if (m_status == SerialStatus::CLOSED)
    {
        return ;
    }
    m_status = SerialStatus::CLOSED;
    if (reader)
    {
        readerThread->quit();
        readerThread->wait();
        delete readerThread;
        delete reader;
    }
    if (writer)
    {
        writer->stopWriting();
        writerThread->quit();
        writerThread->wait();
        delete writerThread;
        delete writer;
    }
    if (serialPort) {
        serialPort->close();
        delete serialPort;
    }
    if (junk_data_thread.joinable())
    {
        junk_data_thread.join();
    }
}

// 处理接收到的数据
void SerialReader::processReceivedData(std::string& receivedData) const
{
    // 处理粘包问题 - 循环处理所有可能的完整数据包
    while (true) {
        // 查找包起始字符 'A'
        size_t startPos = receivedData.find('A');
        if (startPos == std::string::npos) {
            // 没有找到起始符，清空缓冲区
            receivedData.clear();
            break;
        }

        // 如果起始符不在第一个位置，移除前面的无效数据
        if (startPos > 0) {
            receivedData = receivedData.substr(startPos);
        }

        // 查找包结束字符 'B'
        size_t endPos = receivedData.find('B', 1); // 从A后面开始查找
        if (endPos == std::string::npos) {
            // 没有找到完整的数据包，等待更多数据
            break;
        }

        // 确保在B后面有至少一个字符作为包类型
        if (endPos + 1 >= receivedData.length()) {
            // 包不完整，等待更多数据
            break;
        }

        // 提取完整的数据包，确保包括起始A和结束B及其后的类型字符
        std::string packet = receivedData.substr(0, endPos + 2);

        // 从缓冲区中移除已处理的数据包
        receivedData = receivedData.substr(endPos + 2);

        // 处理提取出的数据包

        LOG_DEBUG("接收到数据包: " + packet);

        // 根据类型执行不同操作
        switch (parsePacket(packet)) {
        case PACKET_WIFI_SETUP:
            LOG_INFO("[WiFi 配置提示] 请配置 WiFi");
            break;
        case PACKET_WIFI_SSID_PWD:
            LOG_INFO("[WiFi 配置] 发送 SSID/PWD...");
            break;
        case PACKET_WIFI_CONFIRM:
            LOG_INFO("[WiFi 配置成功]");
            break;
        case PACKET_WIFI_ERROR:
            LOG_INFO("[WiFi 配置错误] SSID 或密码错误");
            break;
        case PACKET_DEVICE_STATUS:
            LOG_INFO("[设备状态] 更新设备信息...");
            break;
        case PACKET_LIGHT_CONTROL:
            LOG_INFO("[补光灯设置] 调整亮度");
            break;
        default:
            ;
            //LOG_WARN("[未知数据包]: " + QString(packet.c_str()));
        }
    }
}
void SerialPortManager::write_data(const std::string& data) const
{
    if (writer)
    {
        writer->sendData(QByteArray::fromStdString(data));
    }
}

static std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos)
        return ""; // 全部为空白
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

PacketType SerialReader::parsePacket(const std::string& packet) const
{
    // 先将收到的字符串去除首尾空白（例如换行符、空格）
    std::string trimmedPacket = trim(packet);
    //LOG_DEBUG("Parsing packet: [" + QString(trimmedPacket.c_str()) + "]");

    // 基本验证：检查包格式是否有效（A开头，B结尾加数字）
    if (trimmedPacket.length() < 3 || trimmedPacket[0] != 'A' ||
        trimmedPacket[trimmedPacket.length() - 2] != 'B') {
        LOG_DEBUG("无效包格式");
        return PACKET_UNKNOWN;
    }

    // 获取包类型（B后面的数字）
    char packetType = trimmedPacket[trimmedPacket.length() - 1];

    std::smatch match;

    // 根据包类型选择正则表达式
    switch (packetType) {
        case '1': // 数据包1：A101B1
            if (std::regex_match(trimmedPacket, std::regex("^A1(01)B1$"))) {
                LOG_DEBUG("匹配到包类型1 (WiFi 配置提示)");
                return PACKET_WIFI_SETUP;
            }
            break;

        case '2': // 数据包2：A2SSID[SSID内容]PWD[PWD]B2
            if (std::regex_match(trimmedPacket, match, std::regex("^A2SSID(.*?)PWD(.*?)B2$"))) {
                LOG_DEBUG("匹配到包类型2 (WiFi 配置数据): SSID = " +
                          match[1].str() + ", PWD = " + match[2].str());
                return PACKET_WIFI_SSID_PWD;
            }
            break;

        case '3': // 数据包3：A303B3
            if (std::regex_match(trimmedPacket, std::regex("^A303B3$"))) {
                LOG_DEBUG("匹配到包类型3 (WiFi 配置成功确认)");
                return PACKET_WIFI_CONFIRM;
            }
            break;

        case '4': // 数据包4：A4SSID[SSID内容]PWD[PWD]B4
            if (std::regex_match(trimmedPacket, match, std::regex("^A4SSID(.*?)PWD(.*?)B4$"))) {
                LOG_DEBUG("匹配到包类型4 (WiFi 配置错误): SSID = " +
                          match[1].str() + ", PWD = " + match[2].str());
                return PACKET_WIFI_ERROR;
            }
            break;

    case '5': // 数据包5：A5[亮度][IP地址]POWER[电量]VERSION[固件版本]B5
        if (std::regex_match(trimmedPacket, match,
            std::regex(R"(^A5(\d{1,3})(\d+)POWER(\d{1,3})VERSION(\d{1,3})B5$)"))) {
            // 获取数字形式的IP地址
            std::string rawIp = match[2];

            // 格式化为标准IPv4地址 (169031168192 -> 169.031.168.192)
            std::string formattedIp;
            try {
                // 确保IP长度足够，如果不够则前面补0
                std::string paddedIp = rawIp;
                while (paddedIp.length() < 12) {
                    paddedIp = "0" + paddedIp;
                }

                // 以3位一组分割，然后转换为标准IPv4格式
                // 修改IP格式化逻辑，反转四组数字的顺序
                formattedIp =
                    std::to_string(std::stoi(paddedIp.substr(9, 3))) + "." +
                    std::to_string(std::stoi(paddedIp.substr(6, 3))) + "." +
                    std::to_string(std::stoi(paddedIp.substr(3, 3))) + "." +
                    std::to_string(std::stoi(paddedIp.substr(0, 3)));
                // 调用回调函数
                if (deviceStatusCallback) {
                    int brightness = std::stoi(match[1]);
                    int power = std::stoi(match[3]);
                    int version = std::stoi(match[4]);
                    deviceStatusCallback(formattedIp, brightness, power, version);
                }
            } catch (const std::exception& e) {
                LOG_ERROR("IP格式转换失败: " + e.what());
                formattedIp = rawIp; // 转换失败时使用原始IP
            }
            LOG_DEBUG("匹配到包类型5 (设备状态): 亮度 = " + match[1].str() +
                      ", IP = " + formattedIp + ", 电量 = " + match[3].str() +
                      ", 固件版本 = " + match[4].str());
            return PACKET_DEVICE_STATUS;
            }
        break;

        case '6': // 数据包6：A6[亮度]B6
            if (std::regex_match(trimmedPacket, match, std::regex("^A6(\\d{1,3})B6$"))) {
                LOG_DEBUG("匹配到包类型6 (补光灯控制): 亮度 = " + match[1].str());
                return PACKET_LIGHT_CONTROL;
            }
            break;

        default:
            break;
    }
    //LOG_DEBUG("未匹配任何数据包类型: " + QString(packetType));
    return PACKET_UNKNOWN;
}
void SerialPortManager::sendWiFiConfig(const std::string& ssid, const std::string& pwd) const
{
    std::string packet = "A2SSID" + ssid + "PWD" + pwd + "B2";
    write_data(packet);
    LOG_INFO("发送 WiFi 配置: " + packet);
}

std::string formatIpAddress(const std::string& ipRaw) {
    // 假设输入格式为类似"169031168192"
    // 如果长度不足，前面补0
    std::string paddedIp = ipRaw;
    while (paddedIp.length() < 12) {
        paddedIp = "0" + paddedIp;
    }

    // 以3位一组分割，然后转换为点分十进制
    std::string formattedIp =
        std::to_string(std::stoi(paddedIp.substr(0, 3))) + "." +
        std::to_string(std::stoi(paddedIp.substr(3, 3))) + "." +
        std::to_string(std::stoi(paddedIp.substr(6, 3))) + "." +
        std::to_string(std::stoi(paddedIp.substr(9, 3)));

    return formattedIp;
}

void SerialPortManager::sendLightControl(int brightness) const
{
    std::string packet = "A6" + std::to_string(brightness) + "B6";
    write_data(packet);
    LOG_INFO("发送补光灯亮度: " + std::to_string(brightness));
}

SerialStatus SerialPortManager::status() const
{
    return m_status;
}

void SerialPortManager::flashESP32(QWidget* window)
{
    // 记录操作
    LOG_INFO("准备刷写ESP32固件...");
    if (m_status == SerialStatus::FAILED)
    {
        LOG_INFO("串口未连接，重启失败");
        QMessageBox::critical(window, "启动失败", "串口未连接");
        return ;
    }

    stop();
    // 不再调用stop()，而是手动关闭程序持有的串口句柄
    // 这样可以释放COM端口而不会导致其他部分的问题
    try {
        // 从SerialPortManager获取端口名
        std::string port = FindEsp32S3Port();
        if (port.empty()) {
            port = "COM2"; // 默认端口
        }
        LOG_INFO("使用端口: " + port);
        // 构造完整的命令路径
        QString appDir = QCoreApplication::applicationDirPath();
        QString esptoolPath = "\"" + appDir + "/esptool.exe\"";
        QString bootloaderPath = "\"" + appDir + "/bootloader.bin\"";
        QString partitionPath = "\"" + appDir + "/partition-table.bin\"";
        QString firmwarePath = "\"" + appDir + "/face_tracker.bin\"";

        // 构造命令
        QString commandStr = QString("%1 --chip ESP32-S3 --port %2 --baud 921600 --before default_reset --after hard_reset write_flash 0x0000 %3 0x8000 %4 0x10000 %5")
            .arg(esptoolPath, port.c_str(), bootloaderPath, partitionPath, firmwarePath);
        LOG_INFO("执行命令: " + commandStr.toStdString());

        // 创建进度窗口
        QProgressDialog progress("正在刷写固件，请稍候...", "取消", 0, 0, window);
        progress.setWindowTitle("固件刷写");
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        progress.setMaximum(0); // 设置为0表示未知进度

        // 使用QProcess执行命令
        QProcess process;

        // 捕获标准输出和错误输出
        window->connect(&process, &QProcess::readyReadStandardOutput, [&]() {
            QString output = process.readAllStandardOutput();
            LOG_INFO(output.trimmed().toStdString());
        });

        window->connect(&process, &QProcess::readyReadStandardError, [&]() {
            QString error = process.readAllStandardError();
            LOG_ERROR("错误: " + error.trimmed().toStdString());
        });

        // 绑定取消按钮
        window->connect(&progress, &QProgressDialog::canceled, [&]() {
            process.kill();
            LOG_WARN("用户取消了固件刷写");
        });

        // 启动进程
        process.start(commandStr);

        // 等待进程启动
        if (!process.waitForStarted(3000)) {
            LOG_ERROR("无法启动esptool.exe: " + process.errorString().toStdString());
            QMessageBox::critical(window, "启动失败", "无法启动esptool.exe: " + process.errorString());
            return;
        }
        LOG_INFO("刷写进程已启动，请等待完成...");

        // 使用事件循环等待进程完成，同时保持UI响应
        QEventLoop loop;
        window->connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
        loop.exec();

        // 进程完成
        progress.setValue(100);

        // 处理结果
        if (process.exitCode() == 0) {
            LOG_INFO("固件刷写成功！");
            QMessageBox::information(window, "刷写完成", "ESP32固件刷写成功！");
        } else {
            LOG_ERROR("固件刷写失败，退出码: " + std::to_string(process.exitCode()));
            QMessageBox::critical(window, "刷写失败", "ESP32固件刷写失败，请检查连接和固件文件！");
        }
        init();
        start();
    } catch (const std::exception& e) {
        LOG_ERROR("发生异常: " + e.what());
        QMessageBox::critical(window, "错误", "刷写过程中发生异常: " + QString(e.what()));
    }
}

void SerialPortManager::restartESP32(QWidget* window)
{
    if (m_status == SerialStatus::FAILED)
    {
        LOG_INFO("串口未连接，重启失败");
        QMessageBox::critical(window, "启动失败", "串口未连接");
        return ;
    }
    // 记录操作
    LOG_INFO("准备重启ESP32设备...");
    stop();

    try {
        // 从SerialPortManager获取端口名
        std::string port = FindEsp32S3Port();
        if (port.empty()) {
            port = "COM2"; // 默认端口
        }

        LOG_INFO("使用端口: " + port);

        // 构造完整的命令路径
        QString appDir = QCoreApplication::applicationDirPath();

        // 构造重启命令 - 只执行重启操作
        QString commandStr = QString("\"%1/esptool.exe\" --port %2 run")
            .arg(appDir, port.c_str());
        LOG_INFO("执行重启命令: " + commandStr.toStdString());

        // 创建进度窗口
        QProgressDialog progress("正在重启设备，请稍候...", "取消", 0, 0, window);
        progress.setWindowTitle("设备重启");
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        progress.setMaximum(0); // 设置为0表示未知进度

        // 使用QProcess执行命令
        QProcess process;

        // 捕获标准输出和错误输出
        window->connect(&process, &QProcess::readyReadStandardOutput, [&]() {
            QString output = process.readAllStandardOutput();
            LOG_INFO(output.trimmed().toStdString());
        });

        window->connect(&process, &QProcess::readyReadStandardError, [&]() {
            QString error = process.readAllStandardError();
            LOG_ERROR("错误： " + error.trimmed().toStdString());
        });

        // 绑定取消按钮
        window->connect(&progress, &QProgressDialog::canceled, [&]() {
            process.kill();
            LOG_INFO("用户取消了设备重启");
        });

        // 启动进程
        process.start(commandStr);

        // 等待进程启动
        if (!process.waitForStarted(3000)) {
            LOG_ERROR("无法启动esptool.exe: " + process.errorString().toStdString());
            QMessageBox::critical(window, "启动失败", "无法启动esptool.exe: " + process.errorString());
            return;
        }

        LOG_INFO("重启进程已启动，请等待完成...");

        // 使用事件循环等待进程完成，同时保持UI响应
        QEventLoop loop;
        window->connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
        loop.exec();

        // 进程完成
        progress.setValue(100);

        // 处理结果
        if (process.exitCode() == 0) {
            LOG_INFO("设备重启成功！");
            QMessageBox::information(window, "重启完成", "ESP32设备重启成功！");
        } else {
            LOG_ERROR("设备重启失败，退出码: " + std::to_string(process.exitCode()));
            QMessageBox::critical(window, "重启失败", "ESP32设备重启失败，请检查连接！");
        }

        // 重新初始化和启动串口
        init();
        start();
    } catch (const std::exception& e) {
        LOG_ERROR("重启过程发生异常: " + e.what());
        QMessageBox::critical(window, "错误", "重启过程中发生异常: " + QString(e.what()));
    }
}
