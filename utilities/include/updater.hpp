#ifndef UPDATER_HPP
#define UPDATER_HPP

#include "logger.hpp"
#include "reflect.hpp"
#include <curl/curl.h>
#include <string>
#include <optional>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

struct ClientVersion
{
    std::string tag;
    std::string description;
    std::string firmware;
    REFLECT(tag, description, firmware);
};

struct Version
{
    ClientVersion version;
    REFLECT(version);
};

struct MemoryStruct {
    char *memory = nullptr;  // 确保初始化为 nullptr
    size_t size = 0;
};

class Updater {
public:
    Updater() {
        curl_handle = curl_easy_init();  // 初始化 cURL 句柄
        if (!curl_handle) {
            throw std::runtime_error("Curl 初始化失败！");
        }
    }

    ~Updater() {
        if (curl_handle) {
            curl_easy_cleanup(curl_handle);  // 清理 cURL 资源
        }
    }

    std::optional<Version> getClientVersion()
    {
        const static std::string url = "http://47.116.163.1/version.json";

        struct MemoryStruct chunk;
        chunk.memory = static_cast<char*>(malloc(1)); // 确保有初始内存
        chunk.size = 0;

        if (curl_handle) {
            curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &Updater::write_memory_callback);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &chunk);
            curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L); // 允许重定向

            auto res = curl_easy_perform(curl_handle);

            Version version;
            if (res != CURLE_OK) {
                 LOG_ERROR("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
            } else {
                std::string json_buffer(chunk.memory, chunk.size);
                version = reflect::json_decode<Version>(json_buffer);
            }

            // 释放资源
            free(chunk.memory);
            return (res == CURLE_OK) ? std::make_optional(version) : std::nullopt;
        } else {
            return std::nullopt;
        }
    }

    std::optional<Version> getCurrentVersion()
    {
        std::ifstream file;
        file.open("version.json");
        if (!file) {
            LOG_ERROR("无法打开文件: version.json");
            return std::nullopt;
        }
        std::string version;
        std::getline(file, version);
        file.close();
        return reflect::json_decode<Version>(version);
    }

    void downloadAppToLocal(QWidget* window)
    {
        auto version = getClientVersion();
        if (!version.has_value()) {
            QMessageBox::critical(window, "错误", "无法获取版本信息");
            return;
        }
        std::string url = "http://47.116.163.1/downloads/" + version->version.tag;
        std::string outputFilename = version->version.tag + ".exe";

        std::ofstream outputFile(outputFilename, std::ios::binary); // 以二进制模式打开文件
        if (!outputFile) {
            QMessageBox::critical(window, "错误", "无法创建文件");
            return ;
        }

        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &Updater::writeData);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &outputFile);

        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK) {
            QMessageBox::critical(window, "错误", "下载失败: " + QString::fromStdString(std::string(curl_easy_strerror(res))));
            return ;
        }

        QMessageBox::information(window, "成功", "文件下载成功");
        return ;
    }

private:
    static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
        size_t realsize = size * nmemb;
        auto *mem = static_cast<MemoryStruct*>(userp);

        // 重新分配内存
        char* ptr = static_cast<char*>(realloc(mem->memory, mem->size + realsize + 1));
        if (ptr == nullptr) {
            // LOG_ERROR("Memory allocation failed");
            return 0;
        }
        mem->memory = ptr;
        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;  // 确保字符串终止符

        return realsize;
    }

    static size_t writeData(void *buffer, size_t size, size_t nmemb, void *userp) {
        auto *outputFile = static_cast<std::ofstream *>(userp);
        outputFile->write(static_cast<const char *>(buffer), size * nmemb);
        return size * nmemb;
    }


    CURL *curl_handle = nullptr;
};

#endif // UPDATER_HPP
