//
// Created by JellyfishKnight on 25-3-2.
//

#ifndef FILE_WRITER_HPP
#define FILE_WRITER_HPP

#include <fstream>
#include <optional>

class WifiCacheFileWriter {
public:
  WifiCacheFileWriter(const std::string& file_path) {
    file_.open(file_path, std::ios::in | std::ios::out);
    if (!file_.is_open()) {
      file_.open(file_path, std::ios::in | std::ios::out | std::ios::trunc);
    }
  }

  std::optional<std::string> try_get_wifi_config() {
    if (!file_.is_open()) {
      return std::nullopt;
    }
    file_.seekg(0);
    std::string ip;
    file_ >> ip;
    return ip;
  }

  void write_wifi_config(const std::string& ip) {
    auto config = try_get_wifi_config();
    if (config.has_value() && config.value() == ip) {
      return;
    }
    if (!file_.is_open()) {
      return;
    }
    file_.seekp(0);
    file_ << ip;
  }

private:
  std::fstream file_;
};



#endif //FILE_WRITER_HPP
