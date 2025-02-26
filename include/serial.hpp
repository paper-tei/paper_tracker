//
// Created by JellyfishKnight on 2025/2/26.
//

#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <windows.h>
#include <string>
#include <functional>
#include <queue>
#include <mutex>
#include <QWindow>

class SerialPortManager {
public:
    SerialPortManager(QWidget* parent = nullptr);
    ~SerialPortManager();
    void start();
    void stopReading();  // ֹͣ���ڶ�ȡ
    void startReading(std::function<void(const std::string&)> logCallback);  // �������ڶ�ȡ
    void restartReading(std::function<void(const std::string&)> logCallback);  // �ָ����ڶ�ȡ
    void writeData(const std::string& data);  // ����д���ݵĽӿ�
    void writeThread();  // ���������е�д������
    HANDLE getHandle() { return hSerial; }

    std::string FindEsp32S3Port();

    void FlashWiFiToESP32(std::wstring ssid, std::wstring password);

    void flashESP32();

    void restart_esp32();

    void FlashExposureToESP32(int exposure);

    void FlashBrightnessToESP32(int brightness);

    std::string extractIPFromSerialData(const std::string& serialData);

private:
    HANDLE hSerial = INVALID_HANDLE_VALUE;  // ���ھ��
    bool running = true;  // �����߳��Ƿ�����
    std::queue<std::string> writeQueue;  // �������ݶ���
    std::mutex writeMutex;  // д������ʱ�Ļ�����
    static DWORD WINAPI readThread(LPVOID param);
    std::function<void(const std::string&)> logCallback;
    std::string port;//S3;

};


#endif //SERIAL_HPP
