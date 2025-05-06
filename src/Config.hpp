#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>

#include <SDL2/SDL.h>

#include "IniReader.hpp"

class Config {
public:
  Config(const std::string &filename = "zoo.ini");
  ~Config();

  std::vector<std::string> getResourcePaths();
  std::string getMenuMusic();
  int getScreenWidth();
  int getScreenHeight();
  std::string getLangDllName();
  SDL_Color getProgressColor();
  SDL_Rect getProgressPosition();
private:
  IniReader * reader = NULL;
};

#endif // CONFIG_HPP