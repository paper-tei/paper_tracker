#pragma once

#include <windows.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include "logger.hpp"

enum SerialStatus
{
    CLOSED = 0,
    OPENED,
    FAILED
};

// 定义数据包类型
enum PacketType {
    PACKET_UNKNOWN = 0,
    PACKET_WIFI_SETUP = 1,     // WiFi配置提示
    PACKET_WIFI_SSID_PWD = 2,  // 发送WiFi SSID/密码
    PACKET_WIFI_CONFIRM = 3,   // WiFi配置成功
    PACKET_WIFI_ERROR = 4,     // WiFi配置错误
    PACKET_DEVICE_STATUS = 5,  // 设备状态
    PACKET_LIGHT_CONTROL = 6   // 补光灯控制
};

class SerialPortManager {
public:
    SerialPortManager();
    ~SerialPortManager();
    // 添加一个回调函数类型
    using DeviceStatusCallback = std::function<void(const std::string& ip, int brightness, int power, int version)>;

    // 添加设置回调的方法
    void setDeviceStatusCallback(DeviceStatusCallback callback);

    [[nodiscard]] std::string getCurrentPort() const { return currentPort; }
    void init();
    void start();
    void stop();
    void write_data(const std::string& data);

    [[nodiscard]] SerialStatus status() const;

    // 发送WiFi配置信息
    static void sendWiFiConfig(HANDLE hSerial, const std::string& ssid, const std::string& pwd);
    
    // 发送补光灯控制命令
    static void sendLightControl(HANDLE hSerial, int brightness);

private:
    // 添加回调函数成员
    DeviceStatusCallback deviceStatusCallback;
    HANDLE hSerial;         // 串口句柄
    bool running;           // 控制线程运行状态
    
    std::thread read_thread;       // 读取线程
    std::thread write_thread;      // 写入线程
    std::thread write_junk_thread; // 测试用的写线程

    // 写入队列
    std::queue<std::string> writeQueue;
    std::mutex writeQueueMutex;
    std::condition_variable writeQueueCV;

    // 初始化串口
    HANDLE initSerialPort(const wchar_t* portName);
    
    // 查找ESP32-S3设备串口
    static std::string FindEsp32S3Port();
    
    // 解析接收的数据包
    [[nodiscard]] PacketType parsePacket(const std::string& packet) const;
    
    // 处理接收到的数据
    void processReceivedData(std::string& receivedData) const;
    std::string currentPort = "COM101"; // 默认端口

    SerialStatus m_status;
};