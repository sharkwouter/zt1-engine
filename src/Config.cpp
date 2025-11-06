#include "Config.hpp"

#include <SDL2/SDL.h>

#include "Utils.hpp"

Config::Config(const std::string &filename) {
  reader = new IniReader(Utils::fixPath(filename));
}

Config::~Config(){

}

std::vector<std::string>  Config::getResourcePaths() {
  std::vector<std::string> resource_paths = std::vector<std::string>();

  std::string path_string = reader->get("resource", "path", "");
  std::string current_path = "";
  for (char character : path_string) {
    if (character == ';') {
      resource_paths.push_back(current_path);
      current_path = "";
      continue;
    }
    current_path += character;
  }
  resource_paths.push_back(current_path);

  for(std::string path : resource_paths) {
    SDL_Log("Found resource path %s", path.c_str());
  }

  return resource_paths;
}

std::string Config::getMenuMusic()
{
  return reader->get("ui", "menuMusic", "");
}

bool Config::getPlayMenuMusic()
{
  return (reader->get("ui", "noMenuMusic") == "0");
}

int Config::getScreenWidth()
{
  return reader->getInt("user", "screenwidth", 800);
}

int Config::getScreenHeight()
{
  return reader->getInt("user", "screenheight", 600);
}

std::string Config::getLangDllName() {
  return reader->get("lib", "lang");
}

std::string Config::getResDllName() {
  return reader->get("lib", "res");
}

SDL_Color Config::getProgressColor() {
  SDL_Color color;

  color.r = (uint8_t) reader->getInt("UI", "progressRed", 255);
  color.g = (uint8_t) reader->getInt("UI", "progressGreen", 255);
  color.b = (uint8_t) reader->getInt("UI", "progressBlue", 255);
  color.a = 255;

  return color;
}

SDL_Rect Config::getProgressPosition() {
  SDL_Rect rect;

  rect.x = reader->getInt("UI", "progressLeft", 0);
  rect.y = reader->getInt("UI", "progressTop", 0);
  rect.w = reader->getInt("UI", "progressRight", 0) - rect.x;
  rect.h = reader->getInt("UI", "progressBottom", 0) - rect.y;

  return rect;
}