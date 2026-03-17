#include "AniFile.hpp"

#include <vector>
#include <cstring>

#include "ZtdFile.hpp"
#include "Utils.hpp"

Animation * AniFile::getAnimation(PalletManager * pallet_manager, const std::string &ztd_file, const std::string &file_name) {
  IniReader * ini_reader = ZtdFile::getIniReader(ztd_file, file_name);

  int width = ini_reader->getInt("animation", "x1") - ini_reader->getInt("animation", "x0");
  int height = ini_reader->getInt("animation", "y1") - ini_reader->getInt("animation", "y0");

  std::unordered_map<std::string, AnimationData *> * animations = new std::unordered_map<std::string, AnimationData *>;
  std::string directory = AniFile::getAnimationDirectory(ini_reader);
  for (std::string direction : ini_reader->getList("animation", "animation")) {
    (*animations)[direction] = AniFile::loadAnimationData(pallet_manager, ztd_file, directory + "/" + direction);
    (*animations)[direction]->width = width;
    (*animations)[direction]->height = height;
  }

  return new Animation(animations);
}

std::string AniFile::getAnimationDirectory(IniReader * ini_reader) {
  std::string directory = ini_reader->get("animation", "dir0");

  std::string dir1 = ini_reader->get("animation", "dir1");
  std::string dir2 = ini_reader->get("animation", "dir2");
  std::string dir3 = ini_reader->get("animation", "dir3");

  if (!dir1.empty()) {
    directory += "/";
    directory += dir1;
  }
  if (!dir2.empty()) {
    directory += "/";
    directory += dir2;
  }
  if (!dir3.empty()) {
    directory += "/";
    directory += dir3;
  }

  return directory;
}

AnimationData * AniFile::loadAnimationData(PalletManager * pallet_manager, const std::string &ztd_file, const std::string &file_name) {
  #ifdef DEBUG
    SDL_Log("Loading %s from %s", file_name.c_str(), ztd_file.c_str());
  #endif
  AnimationData * animation_data = (AnimationData *) calloc(1, sizeof(AnimationData));
  
  int file_size = 0;
  void * file_content = ZtdFile::getFileContent(ztd_file, file_name, &file_size);
  if (file_content == nullptr) {
    SDL_Log("Could not get data from %s in %s, returning nullptr", file_name.c_str(), ztd_file.c_str());
    return nullptr;
  }
  SDL_IOStream * rw = SDL_IOFromMem(file_content, file_size);

  // Read optional header with shadow information
  {
    char header_string[8];
    SDL_ReadIO(rw, header_string, 8);
    if(strncmp(header_string, "FATZ", 8) == 0) {
        #ifdef DEBUG
          SDL_Log("FATZ found");
        #endif
        SDL_ReadU8(rw, &animation_data->has_background);
    } else {
      SDL_SeekIO(rw, 0,SDL_IO_SEEK_SET);
      animation_data->has_background = false;
      #ifdef DEBUG
        SDL_Log("FATZ not found");
      #endif
    }
  }

  SDL_ReadU32LE(rw, &animation_data->frame_time_in_ms);

  uint32_t palette_file_name_length = 0;
  SDL_ReadU32LE(rw, &palette_file_name_length);  // There should be a \0 after the file name string
  char * palette_file_name = (char *) calloc(palette_file_name_length, sizeof(char));
  SDL_ReadIO(rw, palette_file_name, (size_t) palette_file_name_length);
  animation_data->pallet = pallet_manager->getPallet(palette_file_name);

  // Continue reading animation data
  SDL_ReadU32LE(rw, &animation_data->frame_count);

  int frame_count = (int)animation_data->frame_count + (int) animation_data->has_background;
  animation_data->frames = (AnimationFrameData *) calloc(frame_count, sizeof(AnimationFrameData));
  for(int i = 0; i < frame_count; i++) {
    SDL_ReadU32LE(rw, &animation_data->frames[i].size);
    int64_t frame_start = SDL_TellIO(rw);
    SDL_ReadU16LE(rw, &animation_data->frames[i].height);
    SDL_ReadU16LE(rw, &animation_data->frames[i].width);
    SDL_ReadS16LE(rw, &animation_data->frames[i].offset_y);
    SDL_ReadS16LE(rw, &animation_data->frames[i].offset_x);
    SDL_ReadU16LE(rw, &animation_data->frames[i].mystery_bytes);

    int64_t frame_end = frame_start + animation_data->frames[i].size;

    animation_data->frames[i].is_shadow = false;
    if ((animation_data->frames[i].height & 0x8000) > 0) {
      animation_data->frames[i].is_shadow = true;
      animation_data->frames[i].height ^= 0x8000;
    }

    #ifdef DEBUG
      SDL_Log("Frame is %ux%u pixels with offset %i,%i", animation_data->frames[i].width, animation_data->frames[i].height, animation_data->frames[i].offset_x, animation_data->frames[i].offset_y);
      SDL_Log("Mystery bytes are %u and shadow is %i", animation_data->frames[i].mystery_bytes, animation_data->frames[i].is_shadow);
    #endif

    animation_data->frames[i].lines = (AnimationLineData *) calloc(animation_data->frames[i].height, sizeof(AnimationLineData));
    for(int y = 0; y < animation_data->frames[i].height; y++) {
      SDL_ReadU8(rw, &animation_data->frames[i].lines[y].instruction_count);
      animation_data->frames[i].lines[y].instructions = (AnimationDrawInstruction *) calloc(animation_data->frames[i].lines[y].instruction_count, sizeof(AnimationDrawInstruction));
      for(int x = 0; x < animation_data->frames[i].lines[y].instruction_count; x++) {
          SDL_ReadU8(rw, &animation_data->frames[i].lines[y].instructions[x].offset);
          SDL_ReadU8(rw, &animation_data->frames[i].lines[y].instructions[x].color_count);
          if (!animation_data->frames[i].is_shadow) {
            animation_data->frames[i].lines[y].instructions[x].colors = (uint8_t *) calloc(animation_data->frames[i].lines[y].instructions[x].color_count, sizeof(uint8_t));
            SDL_ReadIO(rw, animation_data->frames[i].lines[y].instructions[x].colors, (size_t) animation_data->frames[i].lines[y].instructions[x].color_count);
          } else {
            animation_data->frames[i].lines[y].instructions[x].colors = nullptr;
          }
      }
    }
    SDL_SeekIO(rw, frame_end, SDL_IO_SEEK_SET);
  }
  SDL_CloseIO(rw);
  free(file_content);

  return animation_data;
}