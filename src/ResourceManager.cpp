#include "ResourceManager.hpp"

#include <unordered_map>
#include <filesystem>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "ZtdFile.hpp"

ResourceManager::ResourceManager(Config * config) : config(config) {

}

ResourceManager::~ResourceManager() {

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
  std::string exe_directory = "./";
  char * sdl_base_path = SDL_GetBasePath();
    if (sdl_base_path) {
      exe_directory = std::string(sdl_base_path);
      SDL_free(sdl_base_path);
    }

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

void ResourceManager::load() {
  if (resource_map_loaded) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,"Resource maps was already loaded");
    return;
  }
  std::string current_archive = "";

  SDL_Log("Loading resource map...");

  for (std::string path : config->getResourcePaths()) {
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
        if(file.ends_with(".txt")) {
          SDL_Log("Found resource %s in %s", file.c_str(), current_archive.c_str());
        }
      }
    }
  }
  resource_map_loaded = true;
  SDL_Log("Loading resource map done");
}

void *ResourceManager::getFileContent(const std::string &file_name, int *size) {
  return ZtdFile::getFileContent(getResourceLocation(file_name), file_name, size);
}

SDL_Texture *ResourceManager::getTexture(SDL_Renderer * renderer, const std::string &file_name) {
  SDL_Surface * surface = ZtdFile::getImageSurface(getResourceLocation(file_name), file_name);
  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
}

Mix_Music *ResourceManager::getMusic(const std::string &file_name) {
  return ZtdFile::getMusic(getResourceLocation(file_name), file_name);
}

IniReader ResourceManager::getIniReader(const std::string &file_name)
{
  return ZtdFile::getIniReader(getResourceLocation(file_name), file_name);
}
