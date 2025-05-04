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

  std::string get(const std::string &section, const std::string &key, const std::string &default_value = "");
  std::vector<std::string> getList(const std::string &section, const std::string &key, const std::vector<std::string> &default_value = {});
  int getInt(const std::string &section, const std::string &key, const int &default_value = -1);
  std::vector<int> getIntList(const std::string &section, const std::string &key, const std::vector<int> &default_value = {});

  bool isList(const std::string &section, const std::string &key);
private:
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> content;

  void load(std::string file_content);
};

#endif // INI_READER_HPP