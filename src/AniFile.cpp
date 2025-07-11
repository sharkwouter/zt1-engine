#include "AniFile.hpp"

#include <vector>
#include <cstring>

#include "ZtdFile.hpp"

AniFile::AniFile(const std::string &ztd_file, const std::string &file_name) {
  IniReader * ini_reader = ZtdFile::getIniReader(ztd_file, file_name);

  this->x0 = ini_reader->getInt("animation", "x0");
  this->y0 = ini_reader->getInt("animation", "y0");
  this->x1 = ini_reader->getInt("animation", "x1");
  this->y1 = ini_reader->getInt("animation", "y1");

  std::string directory = this->getAnimationDirectory(ini_reader);
  for (std::string direction : ini_reader->getList("animation", "animation")) {
    SDL_Log("Get animation for direction %s", direction.c_str());
    this->animations[direction] = this->loadAnimation(ztd_file, directory + "/" + direction);
  }
  free(ini_reader);

  this->frame_start_time = SDL_GetTicks();
}

AniFile::~AniFile() {
    for (auto animation_pair : this->animations) {
      for (Frame frame : animation_pair.second.frames) {
        if (frame.surface)
          SDL_FreeSurface(frame.surface);
        if (frame.texture)
          SDL_DestroyTexture(frame.texture);
      }
    }
}

void AniFile::draw(SDL_Renderer *renderer, CompassDirection direction) {
  std::string direction_string = convertCompassDirectionToExistingAnimationString(direction);
  if (direction != this->last_direction) {
    this->last_direction = direction;
    this->current_frame = 0;
    this->frame_start_time = SDL_GetTicks();
  } else {
    if (this->animations[direction_string].frame_time_in_ms < SDL_GetTicks() - this->frame_start_time) {
      // SDL_Log("Next Frame");
      this->current_frame++;
      this->frame_start_time = SDL_GetTicks();
    }
  }

  if (this->current_frame >= this->animations[direction_string].frame_count) {
    this->current_frame = 0;
  }

  int multiplier = 4;

  // Draw shadow
  if (this->animations[direction_string].has_shadow) {
    SDL_Rect shadow_dst = {
      this->x + this->x0 + this->animations[direction_string].frames[this->animations[direction_string].frame_count].x_offset,
      this->y + this->y0 + this->animations[direction_string].frames[this->animations[direction_string].frame_count].y_offset,
      this->animations[direction_string].frames[this->animations[direction_string].frame_count].width * multiplier,
      this->animations[direction_string].frames[this->animations[direction_string].frame_count].height * multiplier,
    };

    if (!this->animations[direction_string].frames[this->animations[direction_string].frame_count].texture) {
      this->animations[direction_string].frames[this->animations[direction_string].frame_count].texture = SDL_CreateTextureFromSurface(renderer, this->animations[direction_string].frames[this->animations[direction_string].frame_count].surface);
    }

    SDL_RenderCopy(renderer, this->animations[direction_string].frames[this->animations[direction_string].frame_count].texture, NULL, &shadow_dst);
  }

  // Draw object
  SDL_Rect dst = {
    this->x + this->x0 + this->animations[direction_string].frames[this->current_frame].x_offset,
    this->y + this->y0 + this->animations[direction_string].frames[this->current_frame].y_offset,
    this->animations[direction_string].frames[this->current_frame].width * multiplier,
    this->animations[direction_string].frames[this->current_frame].height * multiplier,
  };

  SDL_Log("Frame %i/%i image at %i,%i size %ix%i", this->current_frame + 1, this->animations[direction_string].frame_count, dst.x, dst.y, dst.w, dst.h);

  if (!this->animations[direction_string].frames[this->current_frame].texture) {
    this->animations[direction_string].frames[this->current_frame].texture = SDL_CreateTextureFromSurface(renderer, this->animations[direction_string].frames[this->current_frame].surface);
  }

  SDL_RenderCopy(renderer, this->animations[direction_string].frames[this->current_frame].texture, NULL, &dst);
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
        this->draw_mirrored = false;
      } else {
        direction_string = "Z";
        this->draw_mirrored = true;
      }
      break;
    case CompassDirection::NE:
      if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->draw_mirrored = false;
      } else if (this->animations.contains("NW")) {
        direction_string = "NW";
        this->draw_mirrored = true;
      } else {
        direction_string = "N";
      }
      break;
    case CompassDirection::NW:
      if (this->animations.contains("NW")) {
        direction_string = "NW";
        this->draw_mirrored = false;
      } else if (this->animations.contains("NE")) {
        direction_string = "NE";
        this->draw_mirrored = true;
      } else {
        direction_string = "N";
      }
      break;
    case CompassDirection::S:
      if (this->animations.contains("S")) {
        direction_string = "S";
        this->draw_mirrored = false;
      } else {
        direction_string = "N";
        this->draw_mirrored = true;
      }
      break;
    case CompassDirection::SE:
      if (this->animations.contains("SE")) {
        direction_string = "SE";
        this->draw_mirrored = false;
      } else if (this->animations.contains("SW")) {
        direction_string = "SW";
        this->draw_mirrored = true;
      } else {
        direction_string = "S";
      }
      break;
    case CompassDirection::SW:
      if (this->animations.contains("SW")) {
        direction_string = "SW";
        this->draw_mirrored = false;
      } else if (this->animations.contains("SE")) {
        direction_string = "SE";
        this->draw_mirrored = true;
      } else {
        direction_string = "S";
      }
      break;
    case CompassDirection::E:
      if (this->animations.contains("E")) {
        direction_string = "E";
        this->draw_mirrored = false;
      } else {
        direction_string = "W";
        this->draw_mirrored = true;
      }
      break;
    case CompassDirection::W:
      if (this->animations.contains("W")) {
        direction_string = "W";
        this->draw_mirrored = false;
      } else {
        direction_string = "E";
        this->draw_mirrored = true;
      }
    default:
      SDL_Log("Unhandled direction used!");
      break;
  }

  if (!this->animations.contains(direction_string)) {
    SDL_Log("Invalid direction asked of graphic, expect a segfault");
  }

  return direction_string;
}

AniFile::Animation AniFile::loadAnimation(const std::string &ztd_file, const std::string &file_name) {
  Animation animation = {};
  
  void * data = ZtdFile::getFileContent(ztd_file, file_name, nullptr);
 
  // Read optional header with shadow information
  if(strncmp((const char *) data, "FATZ", 8) == 0) {
      animation.has_shadow = (bool) *((char *)data + 8);

      // Move pointer to end of optional header
      data = (char*)data + 9;
  } else {
    animation.has_shadow = false;
  }

  animation.frame_time_in_ms = *(uint32_t *) data;
  data = (char*)data + sizeof(uint32_t);

  uint32_t palette_file_name_length =  *(uint32_t *) data;
  data = (char*)data + sizeof(uint32_t);

  char * palette_file_name = (char *) calloc(palette_file_name_length, sizeof(char));
  strncpy(palette_file_name, (char *) data, palette_file_name_length + 1);
  data = (char*)data + ((palette_file_name_length) * sizeof(char));  // Also skip empty byte after file name

  // Read the pallete file
  void * pallet_data = ZtdFile::getFileContent(ztd_file, std::string(palette_file_name), nullptr);

  uint32_t pallet_color_count =  *(uint32_t *) data;
  pallet_data = (char*)pallet_data + sizeof(uint32_t);

  animation.frame_count =  *(uint32_t *) data;
  data = (char*)data + sizeof(uint32_t);
  SDL_Log("Frames found: %lu", animation.frame_count);

  for(int i = 0; i < (animation.frame_count + (int) animation.has_shadow); i++) {
    Frame frame = {};
    frame.texture = nullptr;

    uint32_t size =  *(uint32_t *) data;
    data = (char*)data + sizeof(uint32_t);

    void * frame_end = (char*) data + size;

    frame.height =  *(uint16_t *) data;
    data = (char*)data + sizeof(uint16_t);

    bool is_shadow = false;
    if (frame.height & 0xFF == 80) {
      is_shadow = true;
      frame.height = frame.height >> 8;
    }

    frame.width =  *(uint16_t *) data;
    data = (char*)data + sizeof(uint16_t);

    SDL_Log("Frame is %ux%u pixels", frame.height, frame.width);

    frame.surface = SDL_CreateRGBSurfaceWithFormat(0, frame.width, frame.height, 0, SDL_PIXELFORMAT_RGBA32);

    frame.x_offset =  *(int16_t *) data;
    data = (char*)data + sizeof(int16_t);

    frame.y_offset =  *(int16_t *) data;
    data = (char*)data + sizeof(int16_t);

    uint16_t mystery_bytes =  *(uint16_t *) data;
    data = (char*)data + sizeof(uint16_t);
    SDL_Log("Mystery bytes are %u", mystery_bytes);

    for(int y = 0; y < frame.height; y++) {
      uint8_t draw_instructions =  *(uint8_t *) data;
      data = (char*)data + sizeof(uint8_t);

      int current_offset = 0;
      for(int instruction = 0; instruction < draw_instructions; instruction++) {
          uint8_t offset_value =  (*(uint8_t *) data);
          data = (char*)data + sizeof(uint8_t);
          current_offset += offset_value;

          uint8_t pixels_to_draw =  *(uint8_t *) data;
          data = (char*)data + sizeof(uint8_t);

          for(int x = 0; x < pixels_to_draw; x++) {
            if (is_shadow) {
              ((uint32_t *) frame.surface->pixels)[frame.width * y + current_offset + x] = 0x000000FF;
            } else {
              ((uint32_t *) frame.surface->pixels)[frame.width * y + current_offset + x] = ((uint32_t *) pallet_data)[*(uint8_t *)data];
            }
            data = (char*)data + sizeof(uint8_t);
          }
          current_offset += pixels_to_draw;

      }
    }

    animation.frames.push_back(frame);
    data = frame_end;
  }

  return animation;
}
