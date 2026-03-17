#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>

#include <SDL3/SDL.h>

#include "IniReader.hpp"

class Config {
public:
  Config(const std::string &filename = "zoo.ini");
  ~Config();

  std::vector<std::string> getResourcePaths();
  std::string getMenuMusic();
  bool getPlayMenuMusic();
  int getScreenWidth();
  int getScreenHeight();
  std::string getLangDllName();
  std::string getResDllName();
  SDL_Color getProgressColor();
  SDL_FRect getProgressPosition();
private:
  IniReader * reader = NULL;
};

#endif // CONFIG_HPP