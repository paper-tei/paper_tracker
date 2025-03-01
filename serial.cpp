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

SerialPortManager::SerialPortManager() : hSerial(INVALID_HANDLE_VALUE), running(false){

}

SerialPortManager::~SerialPortManager()
{
    stop();
}

std::string SerialPortManager::FindEsp32S3Port()
{
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    char portName[256] = { 0 };
    char hardwareId[256] = { 0 };

    // 获取所有 COM 端口设备
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        return "";
    }

    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
        // 获取设备的硬件 ID
        SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardwareId, sizeof(hardwareId), NULL);

        // ESP32-S3 的 VID 和 PID（你的设备管理器截图里是 303A:1001）
        if (strstr(hardwareId, "VID_303A") && strstr(hardwareId, "PID_1001")) {
            // 获取端口名称
            SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)portName, sizeof(portName), NULL);

            // 提取 "COMxxx" 部分
            std::string portStr(portName);
            size_t start = portStr.find("(COM");
            size_t end = portStr.find(")", start);
            if (start != std::string::npos && end != std::string::npos) {
                std::string comPort = portStr.substr(start + 1, end - start - 1);  // 提取 COMxx
                SetupDiDestroyDeviceInfoList(hDevInfo);
                return comPort;
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return "";
}


// 转换 std::string 到 std::wstring
std::wstring SerialPortManager::string_to_wstring(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

void SerialPortManager::start() {
    if (logCallback == nullptr)
    {
        minilog::log_error(nullptr, "Error： 没有加载串口回调函数！");
        return ;
    }

    port = FindEsp32S3Port();
    if (port.empty())
    {
        minilog::log_error(nullptr, "Error： 串口号为空，无法打开串口！");
        return ;
    }
    std::wstring wPort = L"\\\\.\\" + string_to_wstring(port);
    hSerial = CreateFileW(wPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        minilog::log_error(nullptr, "Error： 无法打开串口，错误号为：{}！", error);
        return;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    read_thread = std::thread(
        // [this]
        // {
        //     std::string buffer;
        //     char tempBuffer[1024];
        //     DWORD bytesRead;
        //     while (running) {
        //         BOOL result = ReadFile(hSerial, tempBuffer, sizeof(tempBuffer) - 1, &bytesRead, NULL);
        //         if (!result) {
        //             DWORD error = GetLastError();
        //             // 错误处理，判断是否是无效句柄或者错误的命令
        //             if (error == ERROR_INVALID_HANDLE || error == ERROR_BAD_COMMAND) {
        //                 minilog::log_error(log_window,"Error: 串口句柄无效，可能设备断开或串口被关闭");
        //             } else {
        //                 minilog::log_error(log_window,"Error: 读取串口文件失败，错误号：{}", error);
        //             }
        //             // 继续读取下一个数据
        //             continue;
        //         }
        //
        //         if (bytesRead > 0) {
        //             tempBuffer[bytesRead] = '\0';
        //             buffer += tempBuffer;
        //
        //             while (!buffer.empty()) {
        //                 size_t validUtf8End = buffer.find_last_of("\n");
        //                 if (validUtf8End == std::string::npos) {
        //                     break;
        //                 }
        //
        //                 std::string receivedData = buffer.substr(0, validUtf8End + 1);
        //                 buffer.erase(0, validUtf8End + 1);
        //
        //                 // 提取 IP 地址并发送到 UI
        //                 std::string ipAddress = extractIPFromSerialData(receivedData);
        //                 if (!ipAddress.empty()) {
        //                     // 使用 PostMessage 更新 UI 上的文本框控件
        //                     // PostMessage(hwnd, WM_USER + 3, 0, (LPARAM)new std::string(ipAddress));
        //                 }
        //
        //                 // 发送到 UI
        //                 // PostMessage(hwnd, WM_USER + 2, 0, (LPARAM)new std::string(receivedData));
        //             }
        //         }
        //     }
        //
        // }
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

void SerialPortManager::add_read_callback(std::function<void(const std::string&)> logCallback)
{
    this->logCallback = logCallback;
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





