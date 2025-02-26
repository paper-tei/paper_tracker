//
// Created by JellyfishKnight on 2025/2/26.
//
#include "serial.hpp"
#include <windows.h>
#include <windef.h>
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
#include <regex>

static std::string base64_encode(const std::string& input);
SerialPortManager::SerialPortManager(QWidget* parent ) : hSerial(INVALID_HANDLE_VALUE), running(false) {
}

SerialPortManager::~SerialPortManager() {
    stopReading();
}

std::string SerialPortManager::FindEsp32S3Port() {
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    char portName[256] = { 0 };
    char hardwareId[256] = { 0 };

    // 获取所有 COM 端口设备
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) return "";

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
std::wstring StringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

void SerialPortManager::startReading(std::function<void(const std::string&)> callback) {
	port = FindEsp32S3Port();
    if (port.empty()) {
        std::cerr << "Error: 串口号为空，无法打开串口！" << std::endl;
        return;
    }

    std::wstring wPort = L"\\\\.\\" + StringToWString(port);
    hSerial = CreateFileW(wPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cerr << "Failed to open port: " << port << ", Error code: " << error << std::endl;
        return;
    }

    // ✅ 设置串口超时，避免 `ReadFile()` 阻塞
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    logCallback = callback;
    running = true;

    std::thread serialThread(&SerialPortManager::readThread, this);
    serialThread.detach();  // 避免阻塞
}

void SerialPortManager::stopReading() {
    running = false;
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}
void SerialPortManager::writeData(const std::string& data) {
   // std::lock_guard<std::mutex> lock(writeMutex);  // 确保线程安全
    writeQueue.push(data);  // 将数据加入队列
}
void SerialPortManager::writeThread() {
    while (running) {
        if (!writeQueue.empty()) {
            //std::lock_guard<std::mutex> lock(writeMutex);  // 确保线程安全
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
        //Sleep(10);  // 等待一会再继续检查队列
    }
}
// 串口数据读取线程
DWORD WINAPI SerialPortManager::readThread(LPVOID param) {
    SerialPortManager* self = (SerialPortManager*)param;
    std::string buffer;
    char tempBuffer[1024];
    DWORD bytesRead;

    while (self->running) {
        BOOL result = ReadFile(self->hSerial, tempBuffer, sizeof(tempBuffer) - 1, &bytesRead, NULL);
        if (!result) {
            DWORD error = GetLastError();
            std::cerr << "ReadFile 失败，错误代码：" << error << std::endl;

            // 错误处理，判断是否是无效句柄或者错误的命令
            if (error == ERROR_INVALID_HANDLE || error == ERROR_BAD_COMMAND) {
                std::cerr << "串口句柄无效，可能设备断开或串口被关闭！" << std::endl;
                // 显示错误框
                MessageBoxA(NULL, "检测到设备异常断开，关闭软件。", "错误", MB_ICONERROR | MB_OK);
                // 退出程序
               // ExitProcess(1);  // 使用非 0 返回码退出，表示错误
            }
            // 继续读取下一个数据
            continue;
        }

        if (bytesRead > 0) {
            tempBuffer[bytesRead] = '\0';
            buffer += tempBuffer;

            while (!buffer.empty()) {
                size_t validUtf8End = buffer.find_last_of("\n");
                if (validUtf8End == std::string::npos) {
                    break;
                }

                std::string receivedData = buffer.substr(0, validUtf8End + 1);
                buffer.erase(0, validUtf8End + 1);

                // 提取 IP 地址并发送到 UI
                std::string ipAddress = self->extractIPFromSerialData(receivedData);
                if (!ipAddress.empty()) {
                    // 使用 PostMessage 更新 UI 上的文本框控件
                    // PostMessage(hwnd, WM_USER + 3, 0, (LPARAM)new std::string(ipAddress));
                }

                // 发送到 UI
                // PostMessage(hwnd, WM_USER + 2, 0, (LPARAM)new std::string(receivedData));
            }
        }
    }
    return 0;
}

void SerialPortManager::restart_esp32() {
    // 停止串口读取
    stopReading();

    // 构造 esptool.exe 重启命令
    std::string command = "tools\\esptool.exe --port " + port + " run";  // 使用串口执行重启命令

    // 转换 `std::string` 到 `std::wstring`
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wCommand = converter.from_bytes(command);


    minilog::log_info("正在重启ESP32： {}", std::string(wCommand.begin(), wCommand.end()));

    // `CreateProcessW()` 需要可写的 `LPWSTR`，所以转换 `wstring` 为 `wchar_t*`
    std::vector<wchar_t> cmdBuffer(wCommand.begin(), wCommand.end());
    cmdBuffer.push_back('\0');  // 确保以 null 结尾

    // 创建进程
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcessW(NULL, cmdBuffer.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // 显示重启提示框
       // MessageBoxW(hwnd, L"正在重启 ESP32，请在结束前不要关闭窗口！", L"提示", MB_OK | MB_ICONINFORMATION);
        WaitForSingleObject(pi.hProcess, INFINITE);  // 等待重启完成
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        // 显示重启完成的提示框
       // MessageBoxW(hwnd, L"ESP32 重启完成！", L"提示", MB_OK | MB_ICONINFORMATION);
    }
    else {
        minilog::log_error("无法启动 esptool.exe，错误代码：{}", GetLastError());
    }
    restartReading([](const std::string& data) {
        minilog::log_info("串口数据: {}", data);
    });
}

void SerialPortManager::flashESP32( ) {
    stopReading();
    std::string command = "tools\\esptool.exe --chip ESP32-S3 --port " + port +
        " --baud 921600 --before default_reset --after hard_reset write_flash "
        "0x0000 tools\\bootloader.bin "
        "0x8000 tools\\partition-table.bin "
        "0x10000 tools\\WIFI.bin";

    // 转换 `std::string` 到 `std::wstring`
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wCommand = converter.from_bytes(command);

    minilog::log_info("开始烧录 {}", std::string(wCommand.begin(), wCommand.end()));

    // `CreateProcessW()` 需要可写的 `LPWSTR`，所以转换 `wstring` 为 `wchar_t*`
    std::vector<wchar_t> cmdBuffer(wCommand.begin(), wCommand.end());
    cmdBuffer.push_back('\0');  // 确保以 null 结尾

    // 创建进程
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcessW(NULL, cmdBuffer.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // 显示烧录中的提示框
        // MessageBoxW(hwnd, L"正在烧录，请在结束前不要关闭窗口！", L"提示", MB_OK | MB_ICONINFORMATION);
        WaitForSingleObject(pi.hProcess, INFINITE);  // 等待烧录完成
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        // 烧录完成，提示用户
        // MessageBoxW(hwnd, L"烧录完成！即将重启程序！", L"提示", MB_OK | MB_ICONINFORMATION);

        // **重启程序**
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);  // 获取当前程序路径

        STARTUPINFOW siNew = { sizeof(siNew) };
        PROCESS_INFORMATION piNew;

        if (CreateProcessW(exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &siNew, &piNew)) {
            CloseHandle(piNew.hProcess);
            CloseHandle(piNew.hThread);
        }
        else {
            // MessageBoxW(hwnd, L"程序重启失败！请手动重启。", L"错误", MB_OK | MB_ICONERROR);
        }

        ExitProcess(0);  // 终止当前进程
    }
    else {
        minilog::log_error("无法启动 esptool.exe，错误代码：{}", GetLastError());
    }
    restartReading([](const std::string& data) {
        minilog::log_info("串口数据: {}", data);
        });
}

void SerialPortManager::restartReading(std::function<void(const std::string&)> logCallback) {
    startReading(logCallback);  // 重新启动串口读取
    start();
}

void SerialPortManager::start() {
    running = true;

    // 启动写入线程
    std::thread([this]() { writeThread(); }).detach();  // 使用 lambda 启动成员函数

    // 启动读取线程
    std::thread([this]() { readThread(this); }).detach();  // 同样的方式启动读取线程
}

void SerialPortManager::FlashWiFiToESP32(std::wstring ssid, std::wstring password) {
    // 转换为UTF-8编码
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8_ssid = converter.to_bytes(ssid);
    std::string utf8_password = converter.to_bytes(password);

    // Base64编码确保特殊字符可以正确传输
    std::string encoded_ssid = base64_encode(utf8_ssid);
    std::string encoded_password = base64_encode(utf8_password);

    // 拼接数据包
    std::string dataPacket = "SSID:" + encoded_ssid + "\nPASSWORD:" + encoded_password + "\n";

    // 发送数据
    writeData(dataPacket);
    minilog::log_info("WiFi配置信息已发送到ESP32,请5s左右后点击重启按钮重启面捕。");
    restart_esp32();
}

// Base64编码函数(需要添加)
static std::string base64_encode(const std::string& input) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    int in_len = input.length();
    const unsigned char* bytes_to_encode = reinterpret_cast<const unsigned char*>(input.c_str());

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++)
            ret += base64_chars[char_array_4[j]];

        while (i++ < 3)
            ret += '=';
    }
    return ret;
}

void SerialPortManager::FlashExposureToESP32(int exposure) {
    minilog::log_info("正在烧录曝光到ESP32");

    // 将曝光值转换为字符串
    std::string exposureStr = std::to_string(exposure);

    // 拼接数据包
    std::string dataPacket = "EXPOSURE:" + exposureStr + "\n";

    // 将数据加入发送队列
    writeData(dataPacket);
    minilog::log_info("曝光值已发送到 ESP32, 请5s左右后点击重启按钮重启面捕。");
}

void SerialPortManager::FlashBrightnessToESP32(int brightness) {
    minilog::log_info("正在烧录亮度到ESP32");

    // 将亮度值转换为字符串
    std::string brightnessStr = std::to_string(brightness);

    // 拼接数据包
    std::string dataPacket = "BRIGHTNESS:" + brightnessStr + "\n";

    // 将数据加入发送队列
    writeData(dataPacket);
    minilog::log_info("亮度值已发送到 ESP32");
}

std::string SerialPortManager::extractIPFromSerialData(const std::string& serialData) {
    // 定义匹配 HTTP URL 的正则表达式
    std::regex urlRegex(R"(http://\d+\.\d+\.\d+\.\d+)");

    std::smatch match;
    if (std::regex_search(serialData, match, urlRegex)) {
        return match.str();  // 返回匹配到的 IP 地址
    }

    return "";  // 未找到匹配项时返回空字符串
}
