//
// Created by JellyfishKnight on 25-3-4.
//
#include "wifi_cache_file_writer.hpp"

#include <logger.hpp>
#include <fstream>
#include <utility>

WifiCacheFileWriter::WifiCacheFileWriter(std::string file_path) : file_path_(std::move(file_path))
{
    // create file if not exists
    if (!std::filesystem::exists(file_path_)) {
        std::ofstream file(file_path_);
        if (!file.is_open()) {
            LOG_ERROR("错误： 无法创建wifi缓存文件！");
            return;
        }
    }
}

std::optional<std::string> WifiCacheFileWriter::try_get_wifi_config() const
{
    std::ifstream file(file_path_);
    if (!file.is_open()) {
        LOG_ERROR("错误： 打开wifi缓存文件失败！");
        return std::nullopt;
    }
    file.seekg(std::ios_base::beg);
    std::string ip;
    std::getline(file, ip);
    return ip;
}

void WifiCacheFileWriter::write_wifi_config(const std::string& ip) const
{
    auto config = try_get_wifi_config();
    if (config.has_value() && config.value() == ip) {
        return;
    }
    std::ofstream file(file_path_);
    if (!file.is_open()) {
        LOG_ERROR("错误： 打开wifi缓存文件失败！");
        return;
    }
    file << ip;
    if (file.fail()) {
        LOG_ERROR("错误： 写入wifi缓存信息失败！");
        return ;
    }
    file.flush();
}

