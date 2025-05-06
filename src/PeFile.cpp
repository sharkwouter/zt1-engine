#include "PeFile.hpp"

#define LOAD_SCREEN_ID 502
#define LANGUAGE_ID PRL_LANG_EN_US

#include <SDL2/SDL_image.h>

PeFile::PeFile(const std::string &pe_file) {
  std::string pe_path = "./";
  char * sdl_base_path = SDL_GetBasePath();
  if (sdl_base_path) {
    pe_path = std::string(sdl_base_path);
    SDL_free(sdl_base_path);
  }
  pe_path += pe_file;

  this->loader = PeResourceLoader_Open(pe_path.c_str());
}

PeFile::~PeFile() {
  PeResourceLoader_Close(this->loader);
}

SDL_Surface *PeFile::getLoadScreenSurface()
{
  SDL_Surface * surface = NULL;
  uint32_t size = 0;

  uint8_t * data = PeResourceLoader_GetResource(this->loader, PRL_TYPE_BITMAP, LANGUAGE_ID, LOAD_SCREEN_ID, &size);
  if (data) {
    SDL_RWops * rw = SDL_RWFromMem(data, size);
    surface = IMG_LoadTyped_RW(rw, 1, "BMP");
    free(data);
  } else {
    SDL_Log("Could not load content of loading screen");
  }
  return surface;
}

std::vector<uint32_t> PeFile::getStringIds() {
  std::vector<uint32_t> string_ids;
  if (this->loader) {
    uint32_t string_count;
    uint32_t * string_ids_c = PeResourceLoader_GetResourceIds(this->loader, PRL_TYPE_STRING, &string_count);
    string_ids.reserve((size_t) string_count);
    for (uint32_t i = 0; i < string_count; i++) {
      string_ids.push_back(string_ids_c[i]);
    }
    free(string_ids_c);
  }

  return string_ids;
}

std::string PeFile::getString(uint32_t string_id) {
  if (!this->loader) {
    return "";
  }
  uint32_t string_length;
  uint8_t * c_string = PeResourceLoader_GetResource(this->loader, PRL_TYPE_STRING, LANGUAGE_ID, string_id, &string_length);
  if (!c_string) {
    return "";
  }
  std::string result = std::string((char *) c_string);
  free(c_string);

  return result;
}
