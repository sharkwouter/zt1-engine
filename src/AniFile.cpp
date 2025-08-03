#include "AniFile.hpp"

#include <vector>
#include <cstring>

#ifdef DEBUG
  #include <assert.h>
#endif

#include "ZtdFile.hpp"

AniFile::AniFile(const std::string &ztd_file, const std::string &file_name) {
  IniReader * ini_reader = ZtdFile::getIniReader(ztd_file, file_name);

  this->file_name = file_name;
  // SDL_Log("Loading animation %s", file_name.c_str());

  this->x0 = ini_reader->getInt("animation", "x0");
  this->y0 = ini_reader->getInt("animation", "y0");

  this->width = ini_reader->getInt("animation", "x1") - ini_reader->getInt("animation", "x0");
  this->height = ini_reader->getInt("animation", "y1") - ini_reader->getInt("animation", "y0");

  std::string directory = this->getAnimationDirectory(ini_reader);
  for (std::string direction : ini_reader->getList("animation", "animation")) {
    // SDL_Log("Get animation for direction %s", direction.c_str());
    this->animations[direction] = this->loadAnimationData(ztd_file, directory + "/" + direction);
  }
  free(ini_reader);

  this->frame_start_time = SDL_GetTicks();
}

AniFile::~AniFile() {
    for (auto animation_pair : this->animations) {
      // TODO: Actually free stuff here
      // for (Frame frame : animation_pair.second.frames) {
      //   if (frame.surface)
      //     SDL_FreeSurface(frame.surface);
      //   if (frame.texture)
      //     SDL_DestroyTexture(frame.texture);
      // }
    }
}

void AniFile::draw(SDL_Renderer *renderer,  int x, int y, CompassDirection direction) {
  SDL_Rect rect = {x, y, this->width, this->height};
  this->draw(renderer, &rect, direction);
}

void AniFile::draw(SDL_Renderer *renderer,  SDL_Rect * dest_rect, CompassDirection direction) {
  // std::string direction_string = convertCompassDirectionToExistingAnimationString(direction);
  // if (direction_string.empty()) {
  //   SDL_Log("Cannot draw animation because the specified direction does not exist");
  //   return;
  // }

  // if (direction != this->last_direction) {
  //   this->last_direction = direction;
  //   this->current_frame = 0;
  //   this->frame_start_time = SDL_GetTicks();
  // } else {
  //   if (this->animations[direction_string].frame_time_in_ms < SDL_GetTicks() - this->frame_start_time) {
  //     this->current_frame++;
  //     this->frame_start_time = SDL_GetTicks();
  //   }
  // }

  // if (this->current_frame >= this->animations[direction_string].frame_count) {
  //   this->current_frame = 0;
  // }

  // #ifdef DEBUG
  //   SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
  //   SDL_RenderFillRect(renderer, dest_rect);
  // #endif

  // // Draw background
  // if (this->animations[direction_string].has_background) {
  //   if (!this->animations[direction_string].frames[this->animations[direction_string].frame_count].texture) {
  //     this->animations[direction_string].frames[this->animations[direction_string].frame_count].texture = SDL_CreateTextureFromSurface(renderer, this->animations[direction_string].frames[this->animations[direction_string].frame_count].surface);
  //   }
  //   SDL_RenderCopyEx(renderer, this->animations[direction_string].frames[this->animations[direction_string].frame_count].texture, NULL, dest_rect, 0, NULL, this->renderer_flip);
  // }

  // // Draw object
  // #ifdef DEBUG
  //   SDL_Log("Frame %i/%i image at %i,%i size %ix%i", this->current_frame + 1, this->animations[direction_string].frame_count, dest_rect->x, dest_rect->y, dest_rect->w, dest_rect->h);
  // #endif

  // if (!this->animations[direction_string].frames[this->current_frame].texture) {
  //   this->animations[direction_string].frames[this->current_frame].texture = SDL_CreateTextureFromSurface(renderer, this->animations[direction_string].frames[this->current_frame].surface);
  // }

  // SDL_RenderCopyEx(renderer, this->animations[direction_string].frames[this->current_frame].texture, NULL, dest_rect, 0, NULL, this->renderer_flip);
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

std::string AniFile::convertCompassDirectionToExistingAnimationString(CompassDirection direction) {
  std::string direction_string = "";
  
  switch (direction) {
    case CompassDirection::N:
      if (this->animations.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::NE:
      if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::NW:
      if (this->animations.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::S:
      if (this->animations.contains("S")) {
        direction_string = "S";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NW")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::SE:
      if (this->animations.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("S")) {
        direction_string = "S";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::SW:
      if (this->animations.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("S")) {
        direction_string = "S";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("NW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::E:
      if (this->animations.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::W:
      if (this->animations.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (this->animations.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    default:
      SDL_Log("Unhandled direction used!");
      break;
  }

  if (!this->animations.contains(direction_string)) {
    SDL_Log("Invalid direction asked of graphic, expect a segfault");
  }

  return direction_string;
}

static int get_starting_point(void * data_frames_start, int frame_count, bool has_background, int canvas_width, int canvas_height, int * base_frame_offset_x, int * base_frame_offset_y) {
  void * data = data_frames_start;
  int base_frame = 0;
  int current_frame_offset_x = 0;
  int current_frame_offset_y = 0;

  *base_frame_offset_x = 0;
  *base_frame_offset_y = 0;
  for(int i = 0; i < frame_count + (int) has_background; i++) {
    uint32_t size =  *(uint32_t *) data;
    data = (char*)data + sizeof(uint32_t);

    void * frame_end = (char*) data + size;

    uint16_t height =  *(uint16_t *) data;
    data = (char*)data + sizeof(uint16_t);

    uint16_t width =  *(uint16_t *) data;
    data = (char*)data + sizeof(uint16_t);

    bool is_shadow = false;
    if ((height & 0xFF) == 80) {
      is_shadow = true;
      height = height >> 8;
    }

    int16_t offset_y =  *(int16_t *) data;
    data = (char*)data + sizeof(int16_t);

    int16_t offset_x =  *(int16_t *) data;
    data = (char*)data + sizeof(int16_t);

    current_frame_offset_x = (canvas_width / 2) - (width / 2) + offset_x;
    current_frame_offset_y = (canvas_height / 2) - (height / 2) + offset_y;

    if (i == 0 || i == frame_count || current_frame_offset_x > *base_frame_offset_x || current_frame_offset_y > *base_frame_offset_y) {
      *base_frame_offset_x = current_frame_offset_x;
      *base_frame_offset_y = current_frame_offset_y;
      base_frame = i;
    }

    data = frame_end;
  }


  return base_frame;
}

AniFile::Animation AniFile::loadAnimation(const std::string &ztd_file, const std::string &file_name) {
  Animation animation = {};
  
  void * data = ZtdFile::getFileContent(ztd_file, file_name, nullptr);
 
  // Read optional header with shadow information
  if(strncmp((const char *) data, "FATZ", 8) == 0) {
      animation.has_background = (bool) *((char *)data + 8);

      // Move pointer to end of optional header
      data = (char*)data + 9;
  } else {
    animation.has_background = false;
  }

  animation.frame_time_in_ms = *(uint32_t *) data;
  data = (char*)data + sizeof(uint32_t);

  uint32_t palette_file_name_length =  *(uint32_t *) data;
  data = (char*)data + sizeof(uint32_t);

  char * palette_file_name = (char *) calloc(palette_file_name_length, sizeof(char));
  strncpy(palette_file_name, (char *) data, palette_file_name_length);
  data = (char*)data + ((palette_file_name_length) * sizeof(char));  // Also skip empty byte after file name

  // Read the pallete file
  void * pallet_data = ZtdFile::getFileContent(ztd_file, std::string(palette_file_name), nullptr);

  uint32_t pallet_color_count =  *(uint32_t *) data;
  pallet_data = (char*)pallet_data + sizeof(uint32_t);

  animation.frame_count =  *(uint32_t *) data;
  data = (char*)data + sizeof(uint32_t);
  // SDL_Log("Frames found: %lu", animation.frame_count);

  int base_frame_offset_x = 0;
  int base_frame_offset_y = 0;
  int base_frame = get_starting_point(data, animation.frame_count, animation.has_background, this->width, this->height, &base_frame_offset_x, &base_frame_offset_y);
  for(int i = 0; i < (animation.frame_count + (int) animation.has_background); i++) {
    Frame frame = {};
    frame.texture = nullptr;

    uint32_t size =  *(uint32_t *) data;
    data = (char*)data + sizeof(uint32_t);

    void * frame_end = (char*) data + size;

    frame.height =  *(uint16_t *) data;
    data = (char*)data + sizeof(uint16_t);

    frame.width =  *(uint16_t *) data;
    data = (char*)data + sizeof(uint16_t);

    bool is_shadow = false;
    if ((frame.height & 0xFF) == 80) {
      is_shadow = true;
      frame.height = frame.height >> 8;
    }

    frame.surface = SDL_CreateRGBSurfaceWithFormat(0, this->width, this->height, 0, SDL_PIXELFORMAT_RGBA32);

    frame.y_offset =  *(int16_t *) data;
    data = (char*)data + sizeof(int16_t);

    frame.x_offset =  *(int16_t *) data;
    data = (char*)data + sizeof(int16_t);

    // SDL_Log("Frame is %ux%u pixels with offset %i,%i", frame.height, frame.width, frame.x_offset, frame.y_offset);
    // SDL_Log("Canvas size: %ix%i", this->width, this->height);


    uint16_t mystery_bytes =  *(uint16_t *) data;
    data = (char*)data + sizeof(uint16_t);
    // SDL_Log("Mystery bytes are %u", mystery_bytes);

    int start_x = 0;
    int start_y = 0;
    if (i != base_frame && i != (int) animation.frame_count) {
      start_x = base_frame_offset_x - frame.x_offset;
      start_y = base_frame_offset_y - frame.y_offset;
    } else {
      start_x = (this->width / 2) - (frame.width / 2);
      start_y = (this->height / 2) - (frame.height / 2);
    }
    for(int y = start_y; y < start_y + frame.height; y++) {
      uint8_t draw_instructions =  *(uint8_t *) data;
      data = (char*)data + sizeof(uint8_t);

      int x = start_x;
      for(int instruction = 0; instruction < draw_instructions; instruction++) {
          uint8_t offset_value =  (*(uint8_t *) data);
          data = (char*)data + sizeof(uint8_t);
          x += offset_value;

          uint8_t pixels_to_draw =  *(uint8_t *) data;
          data = (char*)data + sizeof(uint8_t);

          for(int p = 0; p < pixels_to_draw; p++, x++) {
            if (x < 0 || x >= this->width || y < 0 || y >= this->height) {
              SDL_Log("Failure to draw %s within the bounds", this->file_name.c_str());
              break;
            }
            if (is_shadow) {
              ((uint32_t *) frame.surface->pixels)[this->width * y + x] = 0x000000FF;
            } else {
              ((uint32_t *) frame.surface->pixels)[this->width * y + x] = ((uint32_t *) pallet_data)[*(uint8_t *)data];
            }
            data = (char*)data + sizeof(uint8_t);
          }
      }
    }

    animation.frames.push_back(frame);
    data = frame_end;
  }

  return animation;
}

AnimationData * AniFile::loadAnimationData(const std::string &ztd_file, const std::string &file_name) {
  AnimationData * animation_data = (AnimationData *) calloc(1, sizeof(AnimationData));
  
  int file_size = 0;
  void * file_content = ZtdFile::getFileContent(ztd_file, file_name, &file_size);
  SDL_RWops * rw = SDL_RWFromMem(file_content, file_size);

  // Read optional header with shadow information
  {
    char header_string[8];
    SDL_RWread(rw, header_string, sizeof(char), 8);
    if(strncmp(header_string, "FATZ", 8) == 0) {
        SDL_Log("FATZ found");
        animation_data->has_background = SDL_ReadU8(rw);
    } else {
      SDL_RWseek(rw, 0,RW_SEEK_SET);
      animation_data->has_background = SDL_FALSE;
      SDL_Log("FATZ not found");
    }
  }

  animation_data->frame_time_in_ms = SDL_ReadLE32(rw);
  SDL_Log("Frame time: %u", animation_data->frame_time_in_ms);

  uint32_t palette_file_name_length = SDL_ReadLE32(rw);  // There should be a \0 after the file name string
  char * palette_file_name = (char *) calloc(palette_file_name_length, sizeof(char));
  SDL_RWread(rw, palette_file_name, sizeof(char), (size_t) palette_file_name_length);

  // Read the pallete file
  if (pallet_colors == nullptr) {
      SDL_Log("Palette file name: %s",palette_file_name);

  int pallet_file_size = 0;
  void * pallet_file_content = ZtdFile::getFileContent(ztd_file, std::string(palette_file_name), &pallet_file_size);
  if (pallet_file_content == NULL) {
    SDL_Log("Failed to load pallet, segfault inc");
    return NULL;
  }
  SDL_RWops * pallet_rw = SDL_RWFromMem(pallet_file_content, pallet_file_size);

  pallet_color_count =  SDL_ReadLE32(pallet_rw);
  pallet_colors = (uint32_t *) calloc(animation_data->pallet_color_count, sizeof(uint32_t));
  SDL_RWread(pallet_rw, pallet_colors, sizeof(uint32_t), (size_t) animation_data->pallet_color_count);
  SDL_RWclose(pallet_rw);
  }
  animation_data->pallet_color_count = pallet_color_count;
  animation_data->pallet_colors = pallet_colors;

  // Continue reading animation data
  animation_data->frame_count =  SDL_ReadLE32(rw);

  int frame_count = (int)animation_data->frame_count + (int) animation_data->has_background;
  SDL_Log("Found %i frames %u", frame_count, animation_data->frame_count);
  animation_data->frames = (AnimationFrameInfo *) calloc(frame_count, sizeof(AnimationFrameInfo));
  for(int i = 0; i < frame_count; i++) {
    int64_t frame_start = SDL_RWtell(rw);

    SDL_Log("Reading some of the empty data from the metadata %u, %u, %u",animation_data->frames[i].metadata.size, animation_data->frames[i].metadata.height, animation_data->frames[i].metadata.width);
    SDL_RWread(rw, &(animation_data->frames[i].metadata), sizeof(AnimationFrameMetaData), 1);

    int64_t frame_end = frame_start + animation_data->frames[i].metadata.size;

    animation_data->frames[i].is_shadow = false;
    if ((animation_data->frames[i].metadata.height & 0xFF) == 80) {
      animation_data->frames[i].is_shadow = true;
      animation_data->frames[i].metadata.height >>= 8;
    }

    SDL_Log("Frame is %ux%u pixels with offset %i,%i", animation_data->frames[i].metadata.height, animation_data->frames[i].metadata.width, animation_data->frames[i].metadata.offset_x, animation_data->frames[i].metadata.offset_y);
    SDL_Log("Mystery bytes are %u", animation_data->frames[i].metadata.mystery_bytes);

    animation_data->frames[i].lines = (AnimationLineInfo *) calloc(animation_data->frames[i].metadata.height, sizeof(AnimationLineInfo));
    for(int y = 0; y < animation_data->frames[i].metadata.height; y++) {
      animation_data->frames[i].lines[y].instruction_count = SDL_ReadU8(rw);
      animation_data->frames[i].lines[y].instructions = (AnimationFrameDrawInstruction *) calloc(animation_data->frames[i].lines[y].instruction_count, sizeof(AnimationFrameDrawInstruction));
      for(int x = 0; x < animation_data->frames[i].lines[y].instruction_count; x++) {
          animation_data->frames[i].lines[y].instructions[x].offset = SDL_ReadU8(rw);
          animation_data->frames[i].lines[y].instructions[x].color_count = SDL_ReadU8(rw);
          animation_data->frames[i].lines[y].instructions[x].colors = (uint8_t *) calloc(animation_data->frames[i].lines[y].instructions[x].color_count, sizeof(uint8_t));
          SDL_RWread(rw, animation_data->frames[i].lines[y].instructions[x].colors, sizeof(uint8_t), animation_data->frames[i].lines[y].instructions[x].color_count);
      }
    }
    SDL_RWseek(rw, frame_end, RW_SEEK_SET);
  }

  return animation_data;
}