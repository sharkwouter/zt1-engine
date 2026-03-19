

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <filesystem>
#include <vector>

#include <SDL3/SDL.h>


#include "Expansion.hpp"

class Utils {
public:
  static std::string getExecutableDirectory() {
    std::string exe_directory = "./";
    const char * sdl_base_path = SDL_GetBasePath();
    if (sdl_base_path) {
      exe_directory = std::string(sdl_base_path);
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

#ifdef _WIN32
  static std::string fixPath(std::string path_string) {
    std::filesystem::path path = std::filesystem::path(path_string);
    if (!path.is_absolute()) {
      path_string = Utils::getZooTycoonPath() + "/" + path;
    }
    return path_string;
  }
#else
  static std::string getCorrectCaseFilename(std::string base_path, std::string file_name) {
    std::string current_file_name = "";
    std::string matching_file = "";
    for (std::filesystem::directory_entry file : std::filesystem::directory_iterator(base_path)) {
      current_file_name = file.path().filename().string();
      if (current_file_name.length() != file_name.length()) {
        continue;
      }

      bool match = true;
      for (size_t i = 0; i < file_name.length(); i++) {
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
    if (path_string.ends_with("/")) {
      // Remove final slash, otherwise the next part fails
      path_string = path_string.substr(0, path_string.length() - 1);
    }
    std::filesystem::path path = std::filesystem::path(path_string);

    std::filesystem::path new_path = std::filesystem::path("/");
    if (!path.is_absolute()) {
      new_path =  std::filesystem::path(Utils::getZooTycoonPath());
    }
    for (std::filesystem::path part : path) {
      if (part == "/" || part == ".") {
        continue;
      }
      std::string new_part = Utils::getCorrectCaseFilename(new_path.string(), part.string());
      if (new_part.empty()) {
        SDL_Log("Path %s does not exist", new_path.string().c_str());
        return "";
      }
      new_path = new_path / new_part;
    }
    return new_path.string();
  }
#endif

  static std::string getExpansionLangDllName(Expansion expansion) {
    switch (expansion) {
      case Expansion::NONE:
        return Utils::getCorrectCaseFilename(Utils::getZooTycoonPath(), "lang0.dll");
        break;
      case Expansion::DINOSAUR_DIGS:
        return Utils::getCorrectCaseFilename(Utils::getZooTycoonPath(), "lang100.dll");
        break;
      case Expansion::MARINE_MANIA:
        return Utils::getCorrectCaseFilename(Utils::getZooTycoonPath(), "lang200.dll");
        break;
      case Expansion::ALL:
        return Utils::getCorrectCaseFilename(Utils::getZooTycoonPath(), "lang200.dll");
        break;
      default:
        return Utils::getCorrectCaseFilename(Utils::getZooTycoonPath(), "lang0.dll");
        break;
    }
  }

  static Expansion getExpansion() {
    if (!Utils::getExpansionLangDllName(Expansion::MARINE_MANIA).empty() && !Utils::getExpansionLangDllName(Expansion::DINOSAUR_DIGS).empty()) {
      return Expansion::ALL;
    }

    if (!Utils::getExpansionLangDllName(Expansion::MARINE_MANIA).empty()) {
      return Expansion::MARINE_MANIA;
    }

    if (!Utils::getExpansionLangDllName(Expansion::DINOSAUR_DIGS).empty()) {
      return Expansion::DINOSAUR_DIGS;
    }

    return Expansion::NONE;
  }

  static std::string getZooTycoonPath() {
    static std::string zoo_tycoon_path = "";
    if (!zoo_tycoon_path.empty()) {
      return zoo_tycoon_path;
    }

    zoo_tycoon_path = Utils::getExecutableDirectory();
    while (!pathContainsZooTycoonFiles(zoo_tycoon_path)) {
      OpenFolderDialogData data = {"", false};
      SDL_ShowOpenFolderDialog(Utils::openFolderDialogCallback, (void *) &data, NULL, NULL, false);
      while (!data.done) {
        SDL_PumpEvents();
      }
      zoo_tycoon_path = data.file_name;
    }
    return zoo_tycoon_path;
  }

private:
  static bool pathContainsZooTycoonFiles(std::string path) {
    if (path.empty()) {
      return false;
    }

    bool contains_all_expected_files = true;
    std::string expected_files[] = {
      "zoo.ini",
      "res0.dll",
      "lang0.dll"
    };
    for (std::string expected_file : expected_files) {
      std::string found_file_path = Utils::getCorrectCaseFilename(path, expected_file);
      if (found_file_path.empty()) {
        contains_all_expected_files = false;
        break;
      }
      SDL_Log("Found file %s in %s", expected_file.c_str(), path.c_str());
    }
    return contains_all_expected_files;
  }

  typedef struct {
    std::string file_name;
    bool done;
  } OpenFolderDialogData;

  static void SDLCALL openFolderDialogCallback(void* userdata, const char* const* filelist, int filter) {
    OpenFolderDialogData * data = (OpenFolderDialogData *) userdata;
    if (!filelist) {
      data->file_name = "";
    } else {
      data->file_name = std::string(*filelist);
    }
    data->done = true;
  }
};

#endif // UTILS_HPP