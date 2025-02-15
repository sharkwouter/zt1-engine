

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
};

#endif // UTILS_HPP