#include "ResourceManager.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>

#include "ZtdFile.hpp"
#include "Utils.hpp"

ResourceManager::ResourceManager(Config * config) : config(config) {

}

ResourceManager::~ResourceManager() {
  if (this->startup_font) {
    if (TTF_WasInit()) {
      TTF_CloseFont(this->startup_font);
    } else {
      free(this->startup_font);
    }
  }
}

std::string ResourceManager::getCorrectCaseFilename(std::string &base_path, std::string file_name) {
  std::string current_file_name = "";
  std::string matching_file = "";
  for (std::filesystem::directory_entry file : std::filesystem::directory_iterator(base_path)) {
    current_file_name = file.path().filename().string();
    if (current_file_name.length() != file_name.length()) {
      continue;
    }

    bool match = true;
    for (int i = 0; i < file_name.length(); i++) {
      if(std::toupper(current_file_name[i]) != std::toupper(file_name[i])) {
        match = false;
        break;
      }
    }
    if (match) {
      matching_file = current_file_name;
      break;
    }
  }

  return matching_file;
}

std::string ResourceManager::fixPath(std::string &path) {
  std::vector<std::string> parts = std::vector<std::string>();
  size_t part_number = 0;

  // Split the path into parts
  for (char character : path) {
    if (character == '/' || character == '\\' ) {
      part_number++;
      continue;
    }
    if (parts.size() == part_number) {
      parts.push_back("");
    }
    parts[part_number] += character;
  }

  // Get the path of the executable
  std::string exe_directory = Utils::getExecutableDirectory();

  // Check if the paths really exist as they are and correct them
  std::string base_dir = exe_directory;
  for(size_t i = 0; i < parts.size(); i++) {
    if(parts[i] == ".") {
      continue;
    }
    base_dir = exe_directory;
    for(size_t j = 0; j < i; j++) {
      base_dir += parts[j] + std::filesystem::path::preferred_separator;
    }
    std::string new_part = getCorrectCaseFilename(base_dir, parts[i]);
    if (!new_part.empty()) {
      parts[i] = new_part;
    }
  }

  std::string fixed_path = exe_directory;
  for(std::string part : parts) {
    fixed_path += std::filesystem::path::preferred_separator + part;
  }

  return fixed_path;
}

std::string ResourceManager::getResourceLocation(const std::string &resource_name)
{
  if (resource_map.count(resource_name) == 0) {
    SDL_Log("Could not find resource %s", resource_name.c_str());
  }

  return resource_map[resource_name];
}

void ResourceManager::load_resource_map(std::atomic<int> * progress) {
  if (resource_map_loaded) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,"Resource maps was already loaded");
    return;
  }
  std::string current_archive = "";

  SDL_Log("Loading resource map...");

  std::vector<std::string> resource_paths = config->getResourcePaths();
  for (std::string path : resource_paths) {
    path = fixPath(path);
    for (std::filesystem::directory_entry archive : std::filesystem::directory_iterator(path)) {
      current_archive = archive.path().string();
      if (!current_archive.ends_with(".ztd")) {
        continue;
      }
      // SDL_Log("Adding resources from %s", archive.path().c_str());
      for (std::string file : ZtdFile::getFileList(current_archive)) {
        if (resource_map.count(file) == 0) {
          resource_map[file] = current_archive;
        }
      }
    }
    *progress += 50 / resource_paths.size();
  }
  resource_map_loaded = true;
  SDL_Log("Loading resource map done");
}

void ResourceManager::load_string_map(std::atomic<int> * progress) {
  std::vector<std::string> lang_dlls;
  for (std::filesystem::directory_entry lang_dll : std::filesystem::directory_iterator(Utils::getExecutableDirectory())) {
    std::string current_dll = lang_dll.path().filename().string();
    if (!current_dll.starts_with("lang") || !current_dll.ends_with(".dll")) {
      continue;
    }
    lang_dlls.push_back(lang_dll.path().string());
  }
  std::sort(lang_dlls.begin(), lang_dlls.end());

  for (std::string lang_dll : lang_dlls) {
    SDL_Log("Loading strings from %s", lang_dll.c_str());
    PeFile pe_file(lang_dll);
    for (uint32_t string_id : pe_file.getStringIds()) {
      std::string string = pe_file.getString(string_id);
      if (!string.empty()) {
        this->string_map[string_id] = string;
      }
    }
    *progress += 50 / lang_dlls.size();
  }
}

void ResourceManager::load_all(std::atomic<int> * progress, std::atomic<bool> * is_done) {
  this->load_resource_map(progress);
  this->load_string_map(progress);
  *is_done = true;
}

void * ResourceManager::getFileContent(const std::string &file_name, int *size) {
  return ZtdFile::getFileContent(getResourceLocation(file_name), file_name, size);
}

SDL_Texture *ResourceManager::getTexture(SDL_Renderer * renderer, const std::string &file_name) {
  SDL_Surface * surface = ZtdFile::getImageSurface(getResourceLocation(file_name), file_name);
  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
}

Mix_Music * ResourceManager::getMusic(const std::string &file_name) {
  return ZtdFile::getMusic(getResourceLocation(file_name), file_name);
}

IniReader * ResourceManager::getIniReader(const std::string &file_name)
{
  return ZtdFile::getIniReader(getResourceLocation(file_name), file_name);
}

SDL_Texture * ResourceManager::getLoadTexture(SDL_Renderer *renderer, const std::string &lang_dll_name) {
  PeFile pe_file(lang_dll_name);
 
  SDL_Surface * surface = pe_file.getLoadScreenSurface();
  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  return texture;
}

SDL_Texture *ResourceManager::getStringTexture(SDL_Renderer *renderer, const std::string &string, SDL_Color color) {
  if (!this->startup_font) {
    this->startup_font = TTF_OpenFont(std::string(Utils::getExecutableDirectory() +"font/Aileron-Regular.otf").c_str(), 24);
  }
  SDL_Surface * surface = TTF_RenderUTF8_Blended(this->startup_font, string.c_str(), color);
  if (surface == NULL) {
      SDL_Log("Couldn't create surface for text %s: %s", string.c_str(), TTF_GetError());
      return NULL;
  }

  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  return texture;
}

std::string ResourceManager::getString(uint32_t string_id) {
  return this->string_map[string_id];
}
