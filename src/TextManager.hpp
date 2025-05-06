#ifndef PE_RESOURCE_LOADER_HPP
#define TEXT_MANAGER_HPP

#include <string>
#include <unordered_map>

class TextManager {
public:
  TextManager(const std::string &path = "lang0.dll");
  ~TextManager();
private:
  void load();

  std::string dll_path;
  std::unordered_map<int, std::string> text_map;
};

#endif // TEXT_MANAGER_HPP