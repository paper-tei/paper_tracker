//
// Created by JellyfishKnight on 25-3-2.
//

#ifndef FILE_WRITER_HPP
#define FILE_WRITER_HPP

#include <filesystem>
#include <optional>

class WifiCacheFileWriter {
public:
  explicit WifiCacheFileWriter(std::string  file_path);

  [[nodiscard]] std::optional<std::string> try_get_wifi_config() const;

  void write_wifi_config(const std::string& ip) const;
private:
  std::string file_path_;
};



#endif //FILE_WRITER_HPP
