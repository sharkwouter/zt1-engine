

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include <SDL2/SDL.h>

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
};

#endif // UTILS_HPP