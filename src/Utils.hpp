

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <filesystem>
#include <vector>

#include <SDL2/SDL.h>


#include "Expansion.hpp"

class Utils {
public:
  static std::string getExecutableDirectory() {
    std::string exe_directory = "./";
    char * sdl_base_path = SDL_GetBasePath();
    if (sdl_base_path) {
      exe_directory = std::string(sdl_base_path);
      SDL_free(sdl_base_path);
    }
    return exe_directory;
  }

  static std::string getFileExtension(const std::string &file_path) {
    std::string extension = "";

    size_t last_dot = file_path.find_last_of(".");
    if(last_dot != std::string::npos && last_dot >= (file_path.length() - 4)) {
      for(size_t i = last_dot + 1; i < file_path.length(); i++) {
        extension += std::toupper(file_path[i]);
      }
    }

    return extension;
  }

  static std::string getFileName(const std::string &file_path) {
    std::string file_name = "";

    size_t last_slash = file_path.find_last_of("/");
    if(last_slash != std::string::npos) {
      for(size_t i = last_slash + 1; i < file_path.length(); i++) {
        file_name += file_path[i];
      }
    }

    return file_name;
  }

  static std::string string_to_lower(const std::string &value) {
    std::string new_string = "";
    for (char character : value) {
      new_string += std::tolower((uint8_t) character);
    }
    return new_string;
  }

  static std::string getCorrectCaseFilename(std::string base_path, std::string file_name) {
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

  static std::string fixPath(std::string path_string) {
    std::filesystem::path path = std::filesystem::path(path_string);

    std::filesystem::path new_path = std::filesystem::path("/");
    if (!path.is_absolute()) {
      new_path =  std::filesystem::path(Utils::getExecutableDirectory());
    }
    for (std::filesystem::path part : path) {
      if (part == "/" || part == ".") {
        continue;
      }
      std::string new_part = Utils::getCorrectCaseFilename(new_path.string(), part.string());
      if (new_part.empty())
        return "";
      new_path = new_path / new_part;
    }
    return new_path.string();
  }

  static std::string getExpansionLangDllPath(Expansion expansion) {
    switch (expansion) {
      case Expansion::NONE:
        return Utils::fixPath("lang0.dll");
        break;
      case Expansion::DINOSAUR_DIGS:
        return Utils::fixPath("lang100.dll");
        break;
      case Expansion::MARINE_MANIA:
        return Utils::fixPath("lang200.dll");
        break;
      case Expansion::ALL:
        return Utils::fixPath("lang200.dll");
        break;
    }
  }

  static Expansion getExpansion() {
    if (!Utils::getExpansionLangDllPath(Expansion::MARINE_MANIA).empty() && !Utils::getExpansionLangDllPath(Expansion::DINOSAUR_DIGS).empty()) {
      return Expansion::ALL;
    }

    if (!Utils::getExpansionLangDllPath(Expansion::MARINE_MANIA).empty()) {
      return Expansion::MARINE_MANIA;
    }

    if (!Utils::getExpansionLangDllPath(Expansion::DINOSAUR_DIGS).empty()) {
      return Expansion::DINOSAUR_DIGS;
    }

    return Expansion::NONE;
  }
};

#endif // UTILS_HPP