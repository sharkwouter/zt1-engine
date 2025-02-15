#include "Config.hpp"

#include <SDL2/SDL.h>

#include "Utils.hpp"

Config::Config(const std::string &filename) {
  reader = new INIReader(Utils::getExecutableDirectory() + filename);
  if (reader->ParseError() != 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not load config file %s. Make sure this binary is in the same directory as the Zoo Tycoon game files", filename.c_str());
    exit(7);
  }
}

Config::~Config(){

}

std::vector<std::string>  Config::getResourcePaths() {
  std::vector<std::string> resource_paths = std::vector<std::string>();

  std::string path_string = reader->Get("resource", "path", "");
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
  return reader->Get("ui", "menuMusic", "");
}
