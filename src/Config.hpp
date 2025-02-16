#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>

#include "IniReader.hpp"

class Config {
public:
  Config(const std::string &filename = "zoo.ini");
  ~Config();

  std::vector<std::string> getResourcePaths();
  std::string getMenuMusic();
private:
  IniReader * reader = NULL;
};

#endif // CONFIG_HPP