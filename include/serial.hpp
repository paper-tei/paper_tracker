//
// Created by Paper-tei on 2025/2/26.
//

#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <string>
#include <functional>
#include <windows.h>
#include <queue>
#include <mutex>
#include <QPlainTextEdit>

enum PacketType {
    PACKET_UNKNOWN = 0,
    PACKET_WIFI_SETUP,
    PACKET_WIFI_SSID_PWD,
    PACKET_WIFI_CONFIRM,
    PACKET_WIFI_ERROR,
    PACKET_DEVICE_STATUS,
    PACKET_LIGHT_CONTROL
};

class SerialPortManager {
public:
    SerialPortManager(QPlainTextEdit* log_window);

    ~SerialPortManager();

    void start();

    void add_read_callback(std::function<void(const std::string&)> logCallback);

    void stop();

    void write_data(const std::string& data);
private:
    std::string FindEsp32S3Port();

    void FlashWiFiToESP32(std::wstring ssid, std::wstring password);

    void flashESP32();


    PacketType parsePacket(const std::string& packet);

    void sendWiFiConfig(HANDLE hSerial, const std::string& ssid, const std::string& pwd);

    void sendLightControl(HANDLE hSerial, int brightness);

    HANDLE initSerialPort(const wchar_t* portName);

    QPlainTextEdit* log_window = nullptr;
    HANDLE hSerial = INVALID_HANDLE_VALUE;
    bool running = false;
    std::queue<std::string> writeQueue;
    std::mutex writeMutex;
    std::string port;
    std::thread read_thread;
    std::thread write_thread;
    std::mutex writeQueueMutex;
    std::condition_variable writeQueueCV;

};





#endif //SERIAL_HPP
