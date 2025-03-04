//
// Created by JellyfishKnight on 25-3-2.
//

#ifndef FILE_WRITER_HPP
#define FILE_WRITER_HPP

#include <filesystem>
#include <fstream>
#include <optional>
#include <windows.h>
#include <iostream>
#include "logger.hpp"

class WifiCacheFileWriter {
public:
  WifiCacheFileWriter(const std::string& file_path);

  std::optional<std::string> try_get_wifi_config();

  void write_wifi_config(const std::string& ip);
private:
  std::string file_path_;
};



#endif //FILE_WRITER_HPP
