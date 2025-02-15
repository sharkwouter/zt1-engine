#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>

#include "lib/INIReader.h"

class Config {
public:
  Config(const std::string &filename = "zoo.ini");
  ~Config();

  std::vector<std::string> getResourcePaths();
  std::string getMenuMusic();
private:
  INIReader * reader = NULL;
};

#endif // CONFIG_HPP