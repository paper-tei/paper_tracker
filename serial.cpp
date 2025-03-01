//
// Created by Paper-tei on 2025/2/26.
//

#pragma once

#include "serial.hpp"
#include <regex>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>    // 包含 GUID_DEVCLASS_PORTS
#include <regstr.h>     // 包含注册表相关定义
#include <iostream>
#include <string>
#include <cstring>      // for strstr()
#include <vector>
#include <locale>
#include <codecvt>
#include <logger.h>
#include <thread>
#define COM_PORT L"COM2"  // 指定要打开的串口号

SerialPortManager::SerialPortManager(QPlainTextEdit* log_window) : hSerial(INVALID_HANDLE_VALUE), running(false),
    log_window(log_window) {
    hSerial = initSerialPort(COM_PORT);
    // minilog::log_info(log_window, "Serial port opened successfully!");
}

SerialPortManager::~SerialPortManager()
{
    stop();
    read_thread.join();
    write_thread.join();
}

std::string SerialPortManager::FindEsp32S3Port()
{
    return "";
}

void SerialPortManager::start() {
    read_thread = std::thread(
        [this]
        {
            char buffer[256];
            DWORD bytesRead;
            std::string receivedData;
            while (true) {
                if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
                    if (bytesRead > 0) {
                        buffer[bytesRead] = '\0';
                        receivedData += buffer;
                        // 判断是否收到完整数据包（使用 B 结尾）
                        if (receivedData.find("B") != std::string::npos) {
                            std::cout << "Received Packet: " << receivedData << std::endl;
                            PacketType type = parsePacket(receivedData);
                            // 根据类型执行不同操作
                            switch (type) {
                                case PACKET_WIFI_SETUP:
                                    std::cout << "[WiFi 配置提示] 请配置 WiFi\n";
                                    break;
                                case PACKET_WIFI_SSID_PWD:
                                    std::cout << "[WiFi 配置] 发送 SSID/PWD...\n";
                                    break;
                                case PACKET_WIFI_CONFIRM:
                                    std::cout << "[WiFi 配置成功]\n";
                                    break;
                                case PACKET_WIFI_ERROR:
                                    std::cout << "[WiFi 配置错误] SSID 或密码错误\n";
                                    break;
                                case PACKET_DEVICE_STATUS:
                                    std::cout << "[设备状态] 更新设备信息...\n";
                                    break;
                                case PACKET_LIGHT_CONTROL:
                                    std::cout << "[补光灯设置] 调整亮度\n";
                                    break;
                                default:
                                    std::cout << "[未知数据包]\n";
                            }

                            receivedData.clear(); // 清空缓冲区
                        }
                    }
                } else {
                    std::cerr << "串口读取错误: " << GetLastError() << std::endl;
                    break;
                }
                Sleep(100);
            }
        }
    );

    write_thread = std::thread(
        [this]
        {
            while (running) {
                if (!writeQueue.empty()) {
                    std::string data = writeQueue.front();  // 获取队列中的数据
                    writeQueue.pop();  // 从队列中移除数据

                    DWORD bytesWritten;
                    BOOL writeResult = WriteFile(hSerial, data.c_str(), data.size(), &bytesWritten, NULL);

                    if (!writeResult) {
                        std::cerr << "串口写入失败，错误代码：" << GetLastError() << std::endl;
                    }
                    else {
                        std::cout << "成功写入数据: " << data << std::endl;
                    }
                }
            }
        }
    );
}

void SerialPortManager::stop()
{
    running = false;
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}

void SerialPortManager::write_data(const std::string& data)
{
    writeQueue.push(data);
}

static std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos)
        return ""; // 全部为空白
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

PacketType SerialPortManager::parsePacket(const std::string& packet)
{
    // 先将收到的字符串去除首尾空白（例如换行符、空格）
    std::string trimmedPacket = trim(packet);
    std::cout << "[Debug] Parsing packet: [" << trimmedPacket << "]" << std::endl;

    std::smatch match;

    // 数据包1：A101B1
    std::regex regexPacket1("^A1(01)B1$");
    if (std::regex_match(trimmedPacket, regexPacket1)) {
        std::cout << "[Debug] Matched Packet 1 (WiFi 配置提示)" << std::endl;
        return PACKET_WIFI_SETUP;
    }

    // 数据包2：A2SSID[SSID内容]PWD[PWD]B2
    std::regex regexPacket2("^A2SSID(.*?)PWD(.*?)B2$");
    if (std::regex_match(trimmedPacket, match, regexPacket2)) {
        std::cout << "[Debug] Matched Packet 2 (WiFi 配置数据): SSID = "
                  << match[1] << ", PWD = " << match[2] << std::endl;
        return PACKET_WIFI_SSID_PWD;
    }

    // 数据包3：A303B3
    std::regex regexPacket3("^A303B3$");
    if (std::regex_match(trimmedPacket, regexPacket3)) {
        std::cout << "[Debug] Matched Packet 3 (WiFi 配置成功确认)" << std::endl;
        return PACKET_WIFI_CONFIRM;
    }

    // 数据包4：A4SSID[SSID内容]PWD[PWD]B4
    std::regex regexPacket4("^A4SSID(.*?)PWD(.*?)B4$");
    if (std::regex_match(trimmedPacket, match, regexPacket4)) {
        std::cout << "[Debug] Matched Packet 4 (WiFi 配置错误): SSID = "
                  << match[1] << ", PWD = " << match[2] << std::endl;
        return PACKET_WIFI_ERROR;
    }

    // 数据包5：A5[亮度][IP地址]POWER[电量]VERSION[固件版本]B5
    // 示例：A550192.168.31.169POWER100VERSION1B5
    // 说明：
    //   - 第1个捕获组 (\d{1,3}) 匹配亮度（0-100的数字）
    //   - 第2个捕获组 (\d{1,3}(?:\.\d{1,3}){3}) 匹配IP地址
    //   - 第3个捕获组 (\d{1,3}) 匹配电量
    //   - 第4个捕获组 (\d{1,3}) 匹配固件版本
    std::regex regexPacket5("^A5(\\d{1,3})(\\d{1,3}(?:\\.\\d{1,3}){3})POWER(\\d{1,3})VERSION(\\d{1,3})B5$");
    if (std::regex_match(trimmedPacket, match, regexPacket5)) {
        std::cout << "[Debug] Matched Packet 5 (设备状态): "
                  << "亮度 = " << match[1]
                  << ", IP = " << match[2]
                  << ", 电量 = " << match[3]
                  << ", 固件版本 = " << match[4] << std::endl;
        return PACKET_DEVICE_STATUS;
    }

    // 数据包6：A6[亮度]B6
    std::regex regexPacket6("^A6(\\d{1,3})B6$");
    if (std::regex_match(trimmedPacket, match, regexPacket6)) {
        std::cout << "[Debug] Matched Packet 6 (补光灯控制), 亮度 = " << match[1] << std::endl;
        return PACKET_LIGHT_CONTROL;
    }

    std::cout << "[Debug] 未匹配任何数据包" << std::endl;
    return PACKET_UNKNOWN;
}

void SerialPortManager::sendWiFiConfig(HANDLE hSerial, const std::string& ssid, const std::string& pwd)
{
    std::string packet = "A2SSID" + ssid + "PWD" + pwd + "B2";
    DWORD bytesWritten;
    WriteFile(hSerial, packet.c_str(), packet.size(), &bytesWritten, NULL);
    std::cout << "发送 WiFi 配置: " << packet << std::endl;
}

void SerialPortManager::sendLightControl(HANDLE hSerial, int brightness)
{
    std::string packet = "A6" + std::to_string(brightness) + "B6";
    DWORD bytesWritten;
    WriteFile(hSerial, packet.c_str(), packet.size(), &bytesWritten, NULL);
    std::cout << "发送补光灯亮度: " << brightness << std::endl;
}

HANDLE SerialPortManager::initSerialPort(const wchar_t* portName)
{
    HANDLE hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "打开串口失败: " << GetLastError() << std::endl;
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "获取串口状态失败\n";
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "配置串口失败\n";
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    // std::cout << "成功打开串口: " << portName << std::endl;
    return hSerial;
}




