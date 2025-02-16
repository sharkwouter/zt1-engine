#ifndef INI_READER_HPP
#define INI_READER_HPP

#include <vector>
#include <string>
#include <unordered_map>

class IniReader {
public:
  IniReader(const std::string &filename);
  IniReader(void * buffer, size_t size);
  ~IniReader();

  std::string get(const std::string &key, const std::string &value, const std::string &default_value);
private:
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> content;

  void load(std::string file_content);
};

#endif // INI_READER_HPP