//
// Created by JellyfishKnight on 25-3-2.
//

#ifndef FILE_WRITER_HPP
#define FILE_WRITER_HPP

#include <filesystem>
#include <fstream>
#include <optional>
#include <windows.h>

class WifiCacheFileWriter {
public:
  WifiCacheFileWriter(const std::string& file_path) : file_path_(file_path)
  {
    // create file if not exists
    if (!std::filesystem::exists(file_path_)) {
      std::ofstream file(file_path_);
      if (!file.is_open()) {
        std::cerr << "Error: Creating file failed!\n";
        return;
      }
    }
  }

  std::optional<std::string> try_get_wifi_config() {
    std::ifstream file(file_path_);
    if (!file.is_open()) {
      std::cerr << "Error: Opening file failed!\n";
      return std::nullopt;
    }
    file.seekg(std::ios_base::beg);
    std::string ip;
    file.read(ip.data(), ip.size());
    return ip;
  }

  void write_wifi_config(const std::string& ip) {
    auto config = try_get_wifi_config();
    if (config.has_value() && config.value() == ip) {
      return;
    }
    std::ofstream file(file_path_);
    if (!file.is_open()) {
      std::cerr << "Error: Opening file failed!\n";
      return;
    }
    file << ip;
    if (file.fail()) {
      std::cerr << "Error: Writing to file failed!\n";
      return ;
    }
    file.flush();
  }

private:
  std::string file_path_;
};



#endif //FILE_WRITER_HPP
