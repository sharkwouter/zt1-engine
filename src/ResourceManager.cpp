#include "ResourceManager.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>

#include "ZtdFile.hpp"
#include "Utils.hpp"
#include "FontManager.hpp"
#include "Expansion.hpp"

ResourceManager::ResourceManager(Config * config) : config(config) {

}

ResourceManager::~ResourceManager() {
  Mix_HaltMusic();
  if (this->intro_music != nullptr){
    Mix_FreeMusic(this->intro_music);
  }
}

std::string ResourceManager::getResourceLocation(const std::string &resource_name) {
  if (this->resource_map.count(resource_name) == 0) {
    std::string resource_name_with_slash = resource_name + "/";
    if(this->resource_map.count(resource_name_with_slash) == 0) {
      SDL_Log("Could not find resource %s", resource_name.c_str());
      return "";
    }
    return this->resource_map[resource_name_with_slash];
  }

  return this->resource_map[resource_name];
}

void ResourceManager::load_resource_map(std::atomic<float> * progress, float progress_goal) {
  if (resource_map_loaded) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,"Resource maps was already loaded");
    return;
  }
  std::string current_archive = "";

  SDL_Log("Loading resource map...");

  std::vector<std::string> resource_paths = config->getResourcePaths();
  float progress_per_resource_path_load = (progress_goal - *progress) / (float) resource_paths.size();
  for (std::string path : resource_paths) {
    path = Utils::fixPath(path);
    if (path.empty())
      continue;
    for (std::filesystem::directory_entry archive : std::filesystem::directory_iterator(path)) {
      current_archive = archive.path().string();
      if (Utils::getFileExtension(current_archive) != "ZTD") {
        continue;
      }
      // SDL_Log("Adding resources from %s", archive.path().c_str());
      for (std::string file : ZtdFile::getFileList(current_archive)) {
        if (resource_map.count(file) == 0) {
          resource_map[file] = current_archive;
        }
      }
    }

    // Increase progress bar position
    if (*progress + progress_per_resource_path_load < progress_goal) {
      *progress = *progress + progress_per_resource_path_load;
    } else {
      *progress = progress_goal;
    }
  }
  resource_map_loaded = true;
  SDL_Log("Loading resource map done");
}

void ResourceManager::load_string_map(std::atomic<float> * progress, float progress_goal) {
  std::vector<std::string> lang_dlls;
  for (std::filesystem::directory_entry lang_dll : std::filesystem::directory_iterator(Utils::getExecutableDirectory())) {
    std::string current_dll = lang_dll.path().filename().string();
    if (!Utils::string_to_lower(current_dll).starts_with("lang") || Utils::getFileExtension(current_dll) != "DLL") {
      continue;
    }
    lang_dlls.push_back(lang_dll.path().string());
  }
  std::sort(lang_dlls.begin(), lang_dlls.end());

  float progress_per_dll_load = (progress_goal - *progress) / (float) lang_dlls.size();
  for (std::string lang_dll : lang_dlls) {
    SDL_Log("Loading strings from %s", lang_dll.c_str());
    PeFile pe_file(lang_dll);
    for (uint32_t string_id : pe_file.getStringIds()) {
      std::string string = pe_file.getString(string_id);
      if (!string.empty()) {
        this->string_map[string_id] = string;
      }
    }

    // Increase progress bar position
    if (*progress + progress_per_dll_load < progress_goal) {
      *progress = *progress + progress_per_dll_load;
    } else {
      *progress = progress_goal;
    }
  }
}

void ResourceManager::load_animation_map(std::atomic<float> * progress, float progress_goal) {
  std::vector<std::string> ani_files;
  for (auto file : this->resource_map) {
    std::string file_name = file.first;
    if(file_name.ends_with(".ani")) {
      ani_files.push_back(file_name);
    }
  }

  float progress_per_animation_load = (progress_goal - *progress) / (float) ani_files.size();
  for (int i = 0; i < ani_files.size(); i++) {
    SDL_Log("Loading animation from %s", ani_files[i].c_str());
    this->animation_map[ani_files[i]] = this->getAnimation(ani_files[i]);

    // Increase progress bar position
    if (*progress + progress_per_animation_load < progress_goal) {
      *progress = *progress + progress_per_animation_load;
    } else {
      *progress = progress_goal;
    }
  }
}

void ResourceManager::load_pallet_map(std::atomic<float> * progress, float progress_goal) {
  for (auto file : this->resource_map) {
    std::string pal_file = file.first;
    if(Utils::getFileExtension(pal_file) == "PAL") {
      std::string ztd_file = file.second;

      this->pallet_manager.addPalletFileToMap(pal_file, ztd_file);
    }
  }

  this->pallet_manager.loadPalletMap(progress, progress_goal);
}

void ResourceManager::load_all(std::atomic<float> * progress, std::atomic<bool> * is_done) {
  std::vector<void(ResourceManager::*)(std::atomic<float> *, float)> load_functions = {
    &ResourceManager::load_resource_map,
    &ResourceManager::load_string_map,
    &ResourceManager::load_pallet_map,
    // &ResourceManager::load_animation_map,
  };

  for (size_t i = 0; i < load_functions.size(); i++) {
    float progress_goal = 100.0 / load_functions.size() * (i + 1);
    (this->*load_functions[i])(progress, progress_goal);
    if (this->resource_map_loaded && this->intro_music == nullptr && this->config->getPlayMenuMusic()) {
      this->intro_music = this->getMusic(this->config->getMenuMusic());
      Mix_PlayMusic(this->intro_music, -1);
    }
  }

  *is_done = true;
}

void * ResourceManager::getFileContent(const std::string &file_name, int *size) {
  return ZtdFile::getFileContent(getResourceLocation(file_name), file_name, size);
}

SDL_Texture * ResourceManager::getTexture(SDL_Renderer * renderer, const std::string &file_name) {
  SDL_Texture * texture = nullptr;
  SDL_Surface * surface = ZtdFile::getImageSurface(getResourceLocation(file_name), file_name);
  if (surface != nullptr) {
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
  }
  return texture;
}

SDL_Cursor * ResourceManager::getCursor(uint32_t cursor_id) {
  PeFile pe_file(this->config->getResDllName());

  SDL_Surface * surface = pe_file.getCursor(cursor_id);
  SDL_Cursor * cursor = SDL_CreateColorCursor(surface, 0, 0);
  SDL_FreeSurface(surface);

  return cursor;
}

Mix_Music * ResourceManager::getMusic(const std::string &file_name) {
  return ZtdFile::getMusic(getResourceLocation(file_name), file_name);
}

IniReader * ResourceManager::getIniReader(const std::string &file_name) {
  return ZtdFile::getIniReader(getResourceLocation(file_name), file_name);
}

Animation *ResourceManager::getAnimation(const std::string &file_name) {
  std::string resource_location = getResourceLocation(file_name);
  if (!resource_location.empty()) {
    return AniFile::getAnimation(&this->pallet_manager, resource_location, file_name);
  } else {
    std::string full_file_name = file_name + ".ani";
    resource_location = getResourceLocation(full_file_name);
    return AniFile::getAnimation(&this->pallet_manager, resource_location, full_file_name);
  }
}

SDL_Texture * ResourceManager::getLoadTexture(SDL_Renderer *renderer) {
  Expansion expansion = Utils::getExpansion();
  uint32_t loading_screen_id = 502;
  std::string lang_dll_path = Utils::getExpansionLangDllPath(expansion);

  if (expansion == Expansion::ALL) {
    loading_screen_id = 505;
  } else if (expansion == Expansion::MARINE_MANIA) {
    loading_screen_id = 504;
  }

  PeFile pe_file(lang_dll_path);
  SDL_Surface * surface = pe_file.getLoadScreenSurface(loading_screen_id);
  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
}

SDL_Texture *ResourceManager::getStringTexture(SDL_Renderer * renderer, const int font, const std::string &string, SDL_Color color) {
  return this->font_manager.getStringTexture(renderer, font, string, color);
}

std::string ResourceManager::getString(uint32_t string_id) {
  return this->string_map[string_id];
}
