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

class SerialPortManager {
public:
    SerialPortManager();

    ~SerialPortManager();

    void start();

    void add_read_callback(std::function<void(const std::string&)> logCallback);

    void stop();

    void write_data(const std::string& data);
private:
    std::string FindEsp32S3Port();

    void FlashWiFiToESP32(std::wstring ssid, std::wstring password);

    void flashESP32();

    void restart_esp32();

    void FlashExposureToESP32(int exposure);

    void FlashBrightnessToESP32(int brightness);

    std::string extractIPFromSerialData(const std::string& serialData);

    static std::string base64_encode(const std::string& input);

    static std::wstring string_to_wstring(const std::string& str);


    QPlainTextEdit* log_window = nullptr;

    HANDLE hSerial = INVALID_HANDLE_VALUE;
    bool running = false;
    std::queue<std::string> writeQueue;
    std::mutex writeMutex;
    std::string port;
    std::function<void(const std::string&)> logCallback;

    std::thread read_thread;
    std::thread write_thread;
};





#endif //SERIAL_HPP
