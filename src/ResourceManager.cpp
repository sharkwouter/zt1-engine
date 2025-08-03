#include "ResourceManager.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>

#include "ZtdFile.hpp"
#include "Utils.hpp"
#include "FontManager.hpp"

ResourceManager::ResourceManager(Config * config) : config(config) {

}

ResourceManager::~ResourceManager() {
  Mix_HaltMusic();
  if (this->intro_music != nullptr){
    Mix_FreeMusic(this->intro_music);
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
      base_dir += parts[j] + "/";
    }
    std::string new_part = getCorrectCaseFilename(base_dir, parts[i]);
    if (!new_part.empty()) {
      parts[i] = new_part;
    }
  }

  std::string fixed_path = exe_directory;
  for(std::string part : parts) {
    fixed_path += "/" + part;
  }

  return fixed_path;
}

std::string ResourceManager::getResourceLocation(const std::string &resource_name) {
  if (this->resource_map.count(resource_name) == 0) {
    std::string resource_name_with_slash = resource_name + "/";
    if(this->resource_map.count(resource_name_with_slash) == 0) {
      SDL_Log("Could not find resource %s", resource_name.c_str());
      #ifdef DEBUG
        for(auto pair : this->resource_map) {
          SDL_Log("%s", pair.first.c_str());
        }
      #endif
      exit(500);
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
    if (!current_dll.starts_with("lang") || !current_dll.ends_with(".dll")) {
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
    this->animation_map[ani_files[i]] = new AniFile(&this->pallet_manager, getResourceLocation(ani_files[i]), ani_files[i]);

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

AniFile * ResourceManager::getAniFile(const std::string &file_name) {
  if(Utils::getFileExtension(file_name) == "ANI") {
    return new AniFile(&this->pallet_manager, getResourceLocation(file_name), file_name);
  } else {
    std::string ani_file_name = file_name + ".ani";
    return new AniFile(&this->pallet_manager, getResourceLocation(ani_file_name), ani_file_name);
  }
}

AnimationData *ResourceManager::getAnimationData(const std::string &file_name)
{
  AniFile * ani_file = nullptr;
  if(Utils::getFileExtension(file_name) == "ANI") {
    ani_file = new AniFile(&this->pallet_manager, getResourceLocation(file_name), file_name);
  } else {
    std::string ani_file_name = file_name + ".ani";
    ani_file = new AniFile(&this->pallet_manager, getResourceLocation(ani_file_name), ani_file_name);
  }
  
  AnimationData * data = nullptr;
  return data;
}

SDL_Texture * ResourceManager::getLoadTexture(SDL_Renderer *renderer) {
  PeFile pe_file(this->config->getLangDllName());

  SDL_Surface * surface = pe_file.getLoadScreenSurface();
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
