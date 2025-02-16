#include "Config.hpp"

#include <SDL2/SDL.h>

#include "Utils.hpp"

Config::Config(const std::string &filename) {
  reader = new IniReader(Utils::getExecutableDirectory() + filename);
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
