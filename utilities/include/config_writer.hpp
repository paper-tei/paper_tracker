//
// Created by JellyfishKnight on 25-3-2.
//

#ifndef FILE_WRITER_HPP
#define FILE_WRITER_HPP

#include <filesystem>
#include <fstream>
#include <optional>
#include <reflect.hpp>
#include <utility>
#include "logger.hpp"

class ConfigWriter {
public:
  explicit ConfigWriter(std::string file_path) : file_path_(std::move(file_path))
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

  template<typename T>
  bool write_config(T t)
  {
    std::ofstream out_file(file_path_);
    if (!out_file.is_open())
    {
      return false;
    }
    auto json_format_t = reflect::json_encode(t);
    out_file << json_format_t;
    if (out_file.fail())
    {
      return false;
    }
    return true;
  }

  template<typename T>
  T get_config()
  {
    std::ifstream in_file(file_path_);
    if (!in_file.is_open())
    {
      return T{};
    }
    std::string json_str;
    in_file.seekg(std::ios_base::beg);
    std::getline(in_file, json_str);
    if (json_str.empty())
    {
      return T{};
    }
    return reflect::json_decode<T>(json_str);
  }

private:
  std::string file_path_;
};



#endif //FILE_WRITER_HPP
