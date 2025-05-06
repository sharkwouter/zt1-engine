#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <string> 

#include "Config.hpp"
#include "ZtdFile.hpp"
#include "IniReader.hpp"
#include "PeFile.hpp"


class ResourceManager {
public:
  Config * config;

  ResourceManager(Config * config);
  ~ResourceManager();

  void load_all();

  void * getFileContent(const std::string &file_name, int * size);
  SDL_Texture * getTexture(SDL_Renderer * renderer, const std::string &file_name);
  Mix_Music *getMusic(const std::string &file_name);
  IniReader getIniReader(const std::string &file_name);
  SDL_Texture * getLoadTexture(SDL_Renderer * renderer, const std::string &lang_dll_name);

private:
  std::unordered_map<std::string, std::string> resource_map;
  bool resource_map_loaded = false;

  std::string getCorrectCaseFilename(std::string &base_path, std::string file_name);
  std::string fixPath(std::string &path);

  std::string getResourceLocation(const std::string &resoure_name);
};

#endif // RESOURCE_MANAGER_HPP