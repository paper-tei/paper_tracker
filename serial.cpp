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

// 修改SerialPortManager的构造函数
SerialPortManager::SerialPortManager(QPlainTextEdit* log_window)
    : hSerial(INVALID_HANDLE_VALUE), running(false), log_window(nullptr) {  // 将log_window设为nullptr

    std::cout << "正在搜索ESP32-S3设备..." << std::endl;

    std::string portName = FindEsp32S3Port();

    if (portName.empty()) {
        std::cout << "无法找到ESP32-S3设备，尝试使用默认端口COM2" << std::endl;
        hSerial = initSerialPort(COM_PORT);
    } else {
        // 转换为宽字符串
        std::wstring wPortName(L"\\\\.\\");
        wPortName += std::wstring(portName.begin(), portName.end());

        std::cout << "尝试连接到端口: " << portName << std::endl;

        hSerial = CreateFileW(
            wPortName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (hSerial != INVALID_HANDLE_VALUE) {
            // 配置串口参数
            DCB dcbSerialParams = {0};
            dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

            if (GetCommState(hSerial, &dcbSerialParams)) {
                dcbSerialParams.BaudRate = CBR_115200;
                dcbSerialParams.ByteSize = 8;
                dcbSerialParams.StopBits = ONESTOPBIT;
                dcbSerialParams.Parity   = NOPARITY;

                if (!SetCommState(hSerial, &dcbSerialParams)) {
                    std::cerr << "配置串口失败，错误码: " << GetLastError() << std::endl;
                    CloseHandle(hSerial);
                    hSerial = INVALID_HANDLE_VALUE;
                }
            } else {
                std::cerr << "获取串口状态失败，错误码: " << GetLastError() << std::endl;
                CloseHandle(hSerial);
                hSerial = INVALID_HANDLE_VALUE;
            }
        }
    }

    if (hSerial != INVALID_HANDLE_VALUE) {
        std::cout << "串口打开成功！" << std::endl;
        running = true;
    } else {
        std::cerr << "串口打开失败！错误码: " << GetLastError() << std::endl;
    }
}

SerialPortManager::~SerialPortManager()
{
    stop();
    read_thread.join();
    write_thread.join();
    write_junk_thread.join();
}

std::string SerialPortManager::FindEsp32S3Port() {
    std::string targetPort = "";

    // 设备标识符 - 支持两种可能的ID格式
    const char* targetDeviceIds[] = {
        "USB\\VID_303A&PID_1001&REV_0101&MI_00",
        "USB\\VID_303A&PID_1001&MI_00"
    };

    // 获取设备信息集句柄
    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVCLASS_PORTS,  // 过滤为串口设备类
        NULL,                  // 不指定枚举器字符串
        NULL,                  // 父窗口句柄
        DIGCF_PRESENT          // 仅获取当前存在的设备
    );

    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "获取设备信息集失败，错误码: " << GetLastError() << std::endl;
        return "";
    }

    // 枚举设备
    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++) {
        // 获取设备实例ID
        char instanceId[256] = {0};
        if (!SetupDiGetDeviceInstanceIdA(hDevInfo, &devInfoData, instanceId, sizeof(instanceId), NULL)) {
            continue;
        }

        std::cout << "检查设备: " << instanceId << std::endl;

        // 检查是否是目标设备 - 尝试所有可能的ID
        bool isTargetDevice = false;
        for (const char* targetDeviceId : targetDeviceIds) {
            if (strstr(instanceId, targetDeviceId) != NULL) {
                isTargetDevice = true;
                std::cout << "匹配成功，找到目标设备: " << targetDeviceId << std::endl;
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

                if (RegQueryValueExA(hKey, "PortName", NULL, &dwType, (LPBYTE)portName, &dwSize) == ERROR_SUCCESS) {
                    targetPort = portName;
                    std::cout << "找到ESP32-S3设备的COM端口: " << targetPort << std::endl;
                }

                RegCloseKey(hKey);
                break;
            }
        }
    }

    // 调试：遍历所有可用的COM端口
    SP_DEVINFO_DATA devInfoData2;
    devInfoData2.cbSize = sizeof(SP_DEVINFO_DATA);

    std::cout << "系统中所有可用的COM端口:" << std::endl;
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

            if (RegQueryValueExA(hKey, "PortName", NULL, &dwType, (LPBYTE)portName, &dwSize) == ERROR_SUCCESS) {
                // 获取友好名称
                char friendlyName[256] = {0};
                DWORD nameSize = sizeof(friendlyName);
                if (SetupDiGetDeviceRegistryPropertyA(
                    hDevInfo,
                    &devInfoData2,
                    SPDRP_FRIENDLYNAME,
                    NULL,
                    (PBYTE)friendlyName,
                    nameSize,
                    NULL)) {
                    std::cout << " - " << portName << " (" << friendlyName << ")" << std::endl;
                } else {
                    std::cout << " - " << portName << std::endl;
                }
            }

            RegCloseKey(hKey);
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    if (targetPort.empty()) {
        std::cerr << "未找到ESP32-S3设备的COM端口" << std::endl;
    }

    return targetPort;
}
void SerialPortManager::start() {
    read_thread = std::thread(
        [this]
        {
            char buffer[256];
            DWORD bytesRead;
            std::string receivedData;
            while (running) {
                if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
                    if (bytesRead > 0) {
                        buffer[bytesRead] = '\0';
                        receivedData += buffer;

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
                            std::cout << "接收到数据包: " << packet << std::endl;
                            PacketType type = parsePacket(packet);

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
                                    std::cout << "[未知数据包] " << packet << "\n";
                            }
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

    write_junk_thread = std::thread(
        [this]
        {
            while (running) {
                std::string junk = "junk data";
                write_data(junk);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    );

    write_thread = std::thread(
        [this]
        {
            while (running) {
                std::unique_lock<std::mutex> lock(writeQueueMutex);
                writeQueueCV.wait_for(lock, std::chrono::milliseconds(100), [this] { return !writeQueue.empty(); });

                if (!writeQueue.empty()) {
                    std::string data = writeQueue.front();  // 获取队列中的数据
                    writeQueue.pop();  // 从队列中移除数据
                    lock.unlock();  // 解锁，避免在写入时长时间锁定

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

    running = true;
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
    {
        std::lock_guard<std::mutex> lock(writeQueueMutex);
        writeQueue.push(data);
    }
    writeQueueCV.notify_one();
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

    // 基本验证：检查包格式是否有效（A开头，B结尾加数字）
    if (trimmedPacket.length() < 3 || trimmedPacket[0] != 'A' ||
        trimmedPacket[trimmedPacket.length() - 2] != 'B') {
        std::cout << "[Debug] 无效包格式" << std::endl;
        return PACKET_UNKNOWN;
    }

    // 获取包类型（B后面的数字）
    char packetType = trimmedPacket[trimmedPacket.length() - 1];

    std::smatch match;

    // 根据包类型选择正则表达式
    switch (packetType) {
        case '1': // 数据包1：A101B1
            if (std::regex_match(trimmedPacket, std::regex("^A1(01)B1$"))) {
                std::cout << "[Debug] 匹配到包类型1 (WiFi 配置提示)" << std::endl;
                return PACKET_WIFI_SETUP;
            }
            break;

        case '2': // 数据包2：A2SSID[SSID内容]PWD[PWD]B2
            if (std::regex_match(trimmedPacket, match, std::regex("^A2SSID(.*?)PWD(.*?)B2$"))) {
                std::cout << "[Debug] 匹配到包类型2 (WiFi 配置数据): SSID = "
                          << match[1] << ", PWD = " << match[2] << std::endl;
                return PACKET_WIFI_SSID_PWD;
            }
            break;

        case '3': // 数据包3：A303B3
            if (std::regex_match(trimmedPacket, std::regex("^A303B3$"))) {
                std::cout << "[Debug] 匹配到包类型3 (WiFi 配置成功确认)" << std::endl;
                return PACKET_WIFI_CONFIRM;
            }
            break;

        case '4': // 数据包4：A4SSID[SSID内容]PWD[PWD]B4
            if (std::regex_match(trimmedPacket, match, std::regex("^A4SSID(.*?)PWD(.*?)B4$"))) {
                std::cout << "[Debug] 匹配到包类型4 (WiFi 配置错误): SSID = "
                          << match[1] << ", PWD = " << match[2] << std::endl;
                return PACKET_WIFI_ERROR;
            }
            break;

        case '5': // 数据包5：A5[亮度][IP地址]POWER[电量]VERSION[固件版本]B5
            if (std::regex_match(trimmedPacket, match,
                std::regex("^A5(\\d{1,3})(\\d{1,3}(?:\\.\\d{1,3}){3})POWER(\\d{1,3})VERSION(\\d{1,3})B5$"))) {
                std::cout << "[Debug] 匹配到包类型5 (设备状态): "
                          << "亮度 = " << match[1]
                          << ", IP = " << match[2]
                          << ", 电量 = " << match[3]
                          << ", 固件版本 = " << match[4] << std::endl;
                return PACKET_DEVICE_STATUS;
            }
            break;

        case '6': // 数据包6：A6[亮度]B6
            if (std::regex_match(trimmedPacket, match, std::regex("^A6(\\d{1,3})B6$"))) {
                std::cout << "[Debug] 匹配到包类型6 (补光灯控制), 亮度 = " << match[1] << std::endl;
                return PACKET_LIGHT_CONTROL;
            }
            break;

        default:
            break;
    }

    std::cout << "[Debug] 未匹配任何数据包类型: " << packetType << std::endl;
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




