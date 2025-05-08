#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <string>
#include <atomic>

#include <SDL2/SDL_ttf.h>

#include "Config.hpp"
#include "ZtdFile.hpp"
#include "IniReader.hpp"
#include "PeFile.hpp"


class ResourceManager {
public:
  Config * config;

  ResourceManager(Config * config);
  ~ResourceManager();

  void load_all(std::atomic<int> * progress, std::atomic<bool> * is_done);

  void * getFileContent(const std::string &file_name, int * size);
  SDL_Texture * getTexture(SDL_Renderer * renderer, const std::string &file_name);
  Mix_Music * getMusic(const std::string &file_name);
  IniReader * getIniReader(const std::string &file_name);
  SDL_Texture * getLoadTexture(SDL_Renderer * renderer, const std::string &lang_dll_name);
  SDL_Texture * getStringTexture(SDL_Renderer * renderer, const std::string &string, SDL_Color color);
  std::string getString(uint32_t string_id);

private:
  std::unordered_map<std::string, std::string> resource_map;
  std::unordered_map<uint32_t, std::string> string_map;
  bool resource_map_loaded = false;
  TTF_Font * startup_font = nullptr;

  std::string getCorrectCaseFilename(std::string &base_path, std::string file_name);
  std::string fixPath(std::string &path);

  std::string getResourceLocation(const std::string &resoure_name);

  void load_resource_map(std::atomic<int> * progress);
  void load_string_map(std::atomic<int> * progress);
};

#endif // RESOURCE_MANAGER_HPP