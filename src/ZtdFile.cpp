#include "ZtdFile.hpp"

#include <zip.h>
#include <stdlib.h>

#include <SDL2/SDL_image.h>

std::vector<std::string> ZtdFile::getFileList(const std::string &ztd_file) {
  std::vector<std::string> files = std::vector<std::string>();

  if(zip_t * file = zip_open(ztd_file.c_str(), 0, NULL)) {
    size_t index = 0;
    struct zip_stat finfo;
    zip_stat_init(&finfo);
    while ((zip_stat_index(file, index, 0, &finfo)) == 0) {
      files.push_back(std::string(finfo.name));
      index++;
    }
    zip_close(file);
  }
  return files;
}

void * ZtdFile::getFileContent(const std::string &ztd_file, const std::string &file_name, int * size) {
  void * content = NULL;
  if(zip_t * file = zip_open(ztd_file.c_str(), 0, NULL)) {
    int index = 0;
    struct zip_stat finfo;
    zip_stat_init(&finfo);
    while ((zip_stat_index(file, index, 0, &finfo)) == 0) {
      if(std::string(finfo.name) == file_name) {
        *size =  finfo.size;
        content = calloc(*size + 1, sizeof(uint8_t));
        zip_file_t * fd = zip_fopen_index(file, index, 0);
        zip_fread(fd, content, *size);
        break;
      }
      index++;
    }
    zip_close(file);
  }
  return content;
}

static std::string get_file_extension(const std::string &file_name) {
  std::string extension = "";

  for(size_t i = file_name.find_last_of(".") + 1; i < file_name.length(); i++) {
    extension += std::toupper(file_name[i]);
  }

  return extension;
}

SDL_Surface * ZtdFile::getImageSurface(const std::string &ztd_file, const std::string &file_name) {
  SDL_Surface * surface = NULL;
  int file_size = 0;

  void * file_content = ZtdFile::getFileContent(ztd_file, file_name, &file_size);
  if (file_content) {
    SDL_RWops * rw = SDL_RWFromMem(file_content, file_size);
    surface = IMG_LoadTyped_RW(rw, 1, get_file_extension(file_name).c_str());
    free(file_content);
  } else {
    SDL_Log("Could not load content of file %s in %s", file_name.c_str(), ztd_file.c_str());
  }

  return surface;
}

Mix_Music * ZtdFile::getMusic(const std::string &ztd_file, const std::string &file_name) {
  Mix_Music * music = NULL;
  int file_size = 0;
  std::string file_extension = get_file_extension(file_name);
  Mix_MusicType music_type = MUS_WAV;

  void * file_content = ZtdFile::getFileContent(ztd_file, file_name, &file_size);
  if (file_content) {
    SDL_RWops * rw = SDL_RWFromMem(file_content, file_size);
    music = Mix_LoadMUSType_RW(rw, MUS_WAV, 1);
  } else {
    SDL_Log("Could not load content of file %s in %s", file_name.c_str(), ztd_file.c_str());
  }

  return music;
}