#pragma once

#include <windows.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <QMutexLocker>
#include <QSerialPort>
#include <QObject>
#include <QSerialPort>
#include <QWaitCondition>
#include <QQueue>
#include <QThread>
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

class SerialReader : public QObject {
public:
    explicit SerialReader(QSerialPort* port, SerialStatus* status, QObject* parent = nullptr) : QObject(parent), serialPort(port), status(status) {}
    // 添加一个回调函数类型
    using DeviceStatusCallback = std::function<void(const std::string& ip, int brightness, int power, int version)>;
    void setDeviceStatusCallback(DeviceStatusCallback callback) {
        deviceStatusCallback = std::move(callback);
    }
public slots:
    void onReadyRead() {
        if (!serialPort)
        {
            *status = SerialStatus::FAILED;
            return;
        }
        QByteArray data = serialPort->readAll();
        if (data.isEmpty())
        {
            return;
        }
        auto receivedData = QString::fromLocal8Bit(data).toStdString();
        processReceivedData(receivedData);
    }

private:
    // 解析接收的数据包
    PacketType parsePacket(const std::string& packet) const;

    // 处理接收到的数据
    void processReceivedData(std::string& receivedData) const;

    QSerialPort* serialPort;
    SerialStatus* status;
    // 添加回调函数成员
    DeviceStatusCallback deviceStatusCallback;
};

class SerialWriter : public QObject {
public:
    explicit SerialWriter(QSerialPort* port, SerialStatus* status, QObject* parent = nullptr)
        : QObject(parent), serialPort(port), stop(false), status(status) {}

    void sendData(const QByteArray& data) {
        QMutexLocker locker(&mutex);
        writeQueue.enqueue(data);
        cond.wakeOne();  // 唤醒写入线程
    }

    void stopWriting() {
        QMutexLocker locker(&mutex);
        stop = true;
        cond.wakeOne();  // 唤醒线程确保它能够退出
    }

public slots:
    void processWriteQueue() {
        while (!stop) {
            QByteArray data;
            {
                QMutexLocker locker(&mutex);
                if (writeQueue.isEmpty()) {
                    cond.wait(&mutex);  // 没数据时等待
                    continue;
                }
                data = writeQueue.dequeue();
            }

            if (serialPort && serialPort->isOpen()) {
                serialPort->write(data);
                serialPort->flush();
            } else
            {
                *status = SerialStatus::FAILED;
            }
            QThread::msleep(50);  // 控制写入频率，避免 CPU 过载
        }
    }
private:
    QSerialPort* serialPort;
    QQueue<QByteArray> writeQueue;
    QMutex mutex;
    QWaitCondition cond;
    bool stop;
    SerialStatus* status;
};


class SerialPortManager : public QObject {
public:
    explicit SerialPortManager(QObject *parent = nullptr);
    ~SerialPortManager() override;
    using DeviceStatusCallback = std::function<void(const std::string& ip, int brightness, int power, int version)>;

    void setDeviceStatusCallback(DeviceStatusCallback callback)
    {
        reader->setDeviceStatusCallback(callback);
    }

    void init();

    void start() const;

    void stop();

    void write_data(const std::string& data) const;

    SerialStatus status() const;

    // 发送WiFi配置信息
    void sendWiFiConfig(const std::string& ssid, const std::string& pwd) const;
    
    // 发送补光灯控制命令
    void sendLightControl(int brightness) const;

    void restartESP32(QWidget* window);

    void flashESP32(QWidget* window);

    // 查找ESP32-S3设备串口
    std::string FindEsp32S3Port();
private:
    std::string currentPort; // 默认端口
    QSerialPort* serialPort;
    SerialStatus m_status;

    SerialReader* reader{};
    SerialWriter* writer{};
    QThread* readerThread{};
    QThread* writerThread{};
};