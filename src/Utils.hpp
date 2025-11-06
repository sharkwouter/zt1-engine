

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <filesystem>

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

    static std::string getExpansionLangDllPath(Expansion expansion) {
      std::string base_path = Utils::getExecutableDirectory();
      switch (expansion) {
        case Expansion::NONE:
          return base_path + "lang0.dll";
          break;
        case Expansion::DINOSAUR_DIGS:
          return base_path + "lang100.dll";
          break;
        case Expansion::MARINE_MANIA:
          return base_path + "lang200.dll";
          break;
        case Expansion::ALL:
          return base_path + "lang200.dll";
          break;
      }
    }

    static Expansion getExpansion() {
      if (std::filesystem::exists(Utils::getExpansionLangDllPath(Expansion::MARINE_MANIA)) && std::filesystem::exists(Utils::getExpansionLangDllPath(Expansion::DINOSAUR_DIGS))) {
        SDL_Log("Found all expansions");
        return Expansion::ALL;
      }

      if (std::filesystem::exists(Utils::getExpansionLangDllPath(Expansion::MARINE_MANIA))) {
        return Expansion::MARINE_MANIA;
      }

      if (std::filesystem::exists(Utils::getExpansionLangDllPath(Expansion::DINOSAUR_DIGS))) {
        return Expansion::DINOSAUR_DIGS;
      }

      return Expansion::NONE;
    }
};

#endif // UTILS_HPP