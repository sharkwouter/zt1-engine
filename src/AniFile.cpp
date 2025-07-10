#include "AniFile.hpp"

#include <vector>
#include <cstring>

#include "ZtdFile.hpp"

AniFile::AniFile(const std::string &ztd_file, const std::string &file_name) {
  this->ini_reader = ZtdFile::getIniReader(ztd_file, file_name);

  SDL_Log("Loading animation of animal");
  int x0 = this->ini_reader->getInt("animation", "x0");
  int y0 = this->ini_reader->getInt("animation", "y0");
  int x1 = this->ini_reader->getInt("animation", "x1");
  int y1 = this->ini_reader->getInt("animation", "y1");

  std::string animation = this->ini_reader->get("animation", "animation");
  std::string dir0 = this->ini_reader->get("animation", "dir0");
  std::string dir1 = this->ini_reader->get("animation", "dir1");
  std::string dir2 = this->ini_reader->get("animation", "dir2");
  std::string dir3 = this->ini_reader->get("animation", "dir3");

  std::string directory = dir0;
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

  SDL_Log("Directory of animal animation: %s", directory.c_str());


  std::vector<std::string> animations = this->ini_reader->getList("animation", "animation");

  for (std::string animation : animations) {
    int file_size = 0;
    void * data = ZtdFile::getFileContent(ztd_file, directory + "/" + animation, &file_size);
    void * data_starting_point = data;

    char extra_frame = 0;
    
    SDL_Log("");
    if(strncmp((const char *) data, "FATZ", 8) == 0) {
        SDL_Log("File with ZTAF header");
        extra_frame = *((char *)data + 8);

        // Move pointer to end of optional header
        data = (char*)data + 9;
    } else {
        SDL_Log("File without ZTAF header");
    }
    SDL_Log("Extra frame set to %d", extra_frame);

    uint32_t speed = *(uint32_t *) data;
    data = (char*)data + sizeof(uint32_t);
    SDL_Log("Animation speed is %lu", speed);

    uint32_t palette_file_name_length =  *(uint32_t *) data;
    data = (char*)data + sizeof(uint32_t);
    SDL_Log("Palette file name length is %lu", palette_file_name_length);

    char * palette_file_name = (char *) calloc(palette_file_name_length, sizeof(char));
    strncpy(palette_file_name, (char *) data, palette_file_name_length + 1);
    data = (char*)data + ((palette_file_name_length) * sizeof(char));  // Skip empty byte after file name
    SDL_Log("Palette file name: %s", palette_file_name);

    void * pallet_data = ZtdFile::getFileContent(ztd_file, std::string(palette_file_name), nullptr);

    uint32_t pallet_color_count =  *(uint32_t *) data;
    pallet_data = (char*)pallet_data + sizeof(uint32_t);
    SDL_Log("Color count is %lu", pallet_color_count);

    uint32_t fame_count =  *(uint32_t *) data;
    data = (char*)data + sizeof(uint32_t);
    SDL_Log("Frame count is %lu", fame_count);

    for(int frame = 0; frame < (fame_count + extra_frame); frame++) {
      SDL_Log("Frame %i:", frame);

      uint32_t size =  *(uint32_t *) data;
      data = (char*)data + sizeof(uint32_t);
      SDL_Log("Frame size is %lu", size);

      void * frame_end = (char*) data + size;

      uint16_t height =  *(uint16_t *) data;
      data = (char*)data + sizeof(uint16_t);

      bool is_shadow = false;
      if (height & 0xFF == 80) {
        SDL_Log("This is probably a shadow");
        is_shadow = true;
        height = height >> 8;
      }

      uint16_t width =  *(uint16_t *) data;
      data = (char*)data + sizeof(uint16_t);

      SDL_Log("Frame is %ux%u pixels", height, width);

      this->image = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

      int16_t x_offset =  *(int16_t *) data;
      data = (char*)data + sizeof(int16_t);
      SDL_Log("X offset is %i", x_offset);

      int16_t y_offset =  *(int16_t *) data;
      data = (char*)data + sizeof(int16_t);
      SDL_Log("Y offset is %i", x_offset);

      uint16_t mystery_bytes =  *(uint16_t *) data;
      data = (char*)data + sizeof(uint16_t);
      SDL_Log("Mystery bytes are %u", x_offset);

      for(int y = 0; y < height; y++) {
        uint8_t draw_instructions =  *(uint8_t *) data;
        data = (char*)data + sizeof(uint8_t);

        int current_offset = 0;
        for(int instruction = 0; instruction < draw_instructions; instruction++) {
            uint8_t offset_value =  (*(uint8_t *) data);
            data = (char*)data + sizeof(uint8_t);
            current_offset += offset_value;

            uint8_t color_count =  *(uint8_t *) data;
            data = (char*)data + sizeof(uint8_t);
            if (y == 31) {
              SDL_Log("offset: %u, color_count: %u",offset_value, color_count);
            }

            for(int color_index = 0; color_index < color_count; color_index++) {
              if (instruction != 0) {
                offset_value = 0;
              }
              // if (offset_value + color_index < width && y == 31) {
                ((uint32_t *) image->pixels)[width * y + current_offset + color_index] = ((uint32_t *) pallet_data)[*(uint8_t *)data];
              // }
              data = (char*)data + sizeof(uint8_t);
            }
            current_offset += color_count;

        }
      }

      SDL_Log("List of pixels should have been rendered now");

      data = frame_end;
    }

    SDL_Log("");
  }
}

AniFile::~AniFile() {
    if (this->ini_reader) {
        free(this->ini_reader);
    }
}

SDL_Texture *AniFile::getTexture(SDL_Renderer *renderer) {
  return nullptr;
}

void AniFile::draw(SDL_Renderer *renderer) {
  if (!this->image) {
    return;
  }

  if (!this->texture) {
    this->texture = SDL_CreateTextureFromSurface(renderer, this->image);
  }

  int multiplier = 4;
  SDL_Rect dst = {0, 0, this->image->w * multiplier, this->image->h * multiplier};
  SDL_RenderCopy(renderer, this->texture, NULL, &dst);
}
