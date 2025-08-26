#include "Animation.hpp"

#include <assert.h>

Animation::Animation(std::unordered_map<std::string, AnimationData *> * data) {
  for(auto map_entry : *data) {
    this->loadSurfaces(map_entry.first, map_entry.second);
  }
}

Animation::~Animation() {
  for (auto surface_list : this->surfaces) {
    for (SDL_Surface * surface : surface_list.second) {
      SDL_FreeSurface(surface);
    }
    surface_list.second.clear();
  }
  for (auto texture_list : this->textures) {
    for (SDL_Texture * texture : texture_list.second) {
      SDL_DestroyTexture(texture);
    }
    texture_list.second.clear();
  }
}

void Animation::draw(SDL_Renderer *renderer,  int x, int y, CompassDirection direction) {
  std::string direction_string = convertCompassDirectionToExistingAnimationString(direction, this->textures);
  SDL_Rect rect = {x, y, 0, 0};
  if (!this->textures[direction_string].empty()) {
    SDL_QueryTexture(this->textures[direction_string][this->current_frame], NULL, NULL, &rect.w, &rect.h);
  } else {
    direction_string = convertCompassDirectionToExistingAnimationString(direction, this->surfaces);
    SDL_Log("Look at surface of frame %i of %i", this->current_frame, this->surfaces[direction_string].size());
    rect.w = this->surfaces[direction_string][this->current_frame]->w;
    rect.h = this->surfaces[direction_string][this->current_frame]->h;
  }
  assert(rect.h > 0);
  assert(rect.w > 0);
  this->draw(renderer, &rect, direction);
}

void Animation::draw(SDL_Renderer *renderer,  SDL_Rect * dest_rect, CompassDirection direction) {
  std::string direction_string = convertCompassDirectionToExistingAnimationString(direction, this->textures);
  if (direction_string.empty()) {
    direction_string = convertCompassDirectionToExistingAnimationString(direction, this->surfaces);
    this->textures[direction_string] = std::vector<SDL_Texture *>();
    if (!direction_string.empty()) {
      for (SDL_Surface * surface: this->surfaces[direction_string]) {
        this->textures[direction_string].push_back(
          SDL_CreateTextureFromSurface(renderer, surface)
        );
        SDL_FreeSurface(surface);
      }
      this->surfaces[direction_string].clear();
    } else {
      SDL_Log("Cannot draw animation because the specified direction does not exist");
      return;
    }
  }
  assert(!this->textures[direction_string].empty());

  if (direction != this->last_direction) {
    this->last_direction = direction;
    this->current_frame = 0;
    this->frame_start_time = SDL_GetTicks();
  } else {
    if (this->frame_time_in_ms < SDL_GetTicks() - this->frame_start_time) {
      this->current_frame++;
      this->frame_start_time = SDL_GetTicks();
    }
  }

  if (this->current_frame >= this->textures[direction_string].size()) {
    this->current_frame = 0;
  }

  #ifdef DEBUG
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
    SDL_RenderFillRect(renderer, dest_rect);
  #endif

  // Draw background
  if (this->has_background) {
    SDL_RenderCopyEx(renderer, this->textures[direction_string][this->textures[direction_string].size() - 1], NULL, dest_rect, 0, NULL, this->renderer_flip);
  }

  // Draw object
  #ifdef DEBUG
    // SDL_Log("Frame %i/%i image at %i,%i size %ix%i", this->current_frame + 1, this->animations[direction_string].frame_count, dest_rect->x, dest_rect->y, dest_rect->w, dest_rect->h);
  #endif

  SDL_RenderCopyEx(renderer, this->textures[direction_string][this->current_frame], NULL, dest_rect, 0, NULL, this->renderer_flip);
}

std::string Animation::convertCompassDirectionToString(CompassDirection direction) {
  std::string direction_string = "";
  
  switch (direction) {
    case CompassDirection::N:
      direction_string = "N";
      break;
    case CompassDirection::NE:
        direction_string = "NE";
      break;
    case CompassDirection::NW:
      direction_string = "NW";
      break;
    case CompassDirection::S:
      direction_string = "S";
      break;
    case CompassDirection::SE:
        direction_string = "SE";
      break;
    case CompassDirection::SW:
        direction_string = "SW";
      break;
    case CompassDirection::E:
        direction_string = "E";
      break;
    case CompassDirection::W:
        direction_string = "W";
        break;
    default:
      SDL_Log("Direction used has no string equivalent direction used!");
      break;
  }

  return direction_string;
}

template <typename T>
std::string Animation::convertCompassDirectionToExistingAnimationString(CompassDirection direction, std::unordered_map<std::string, T> &animation_map) {
  std::string direction_string = "";
  
  switch (direction) {
    case CompassDirection::N:
      if (animation_map.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::NE:
      if (animation_map.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::NW:
      if (animation_map.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::S:
      if (animation_map.contains("S")) {
        direction_string = "S";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NW")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::SE:
      if (animation_map.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("S")) {
        direction_string = "S";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::SW:
      if (animation_map.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("S")) {
        direction_string = "S";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("NW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::E:
      if (animation_map.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    case CompassDirection::W:
      if (animation_map.contains("W")) {
        direction_string = "W";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("E")) {
        direction_string = "E";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("NW")) {
        direction_string = "NW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("SW")) {
        direction_string = "SW";
        this->renderer_flip = SDL_FLIP_NONE;
      } else if (animation_map.contains("NE")) {
        direction_string = "NE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("SE")) {
        direction_string = "SE";
        this->renderer_flip = SDL_FLIP_HORIZONTAL;
      } else if (animation_map.contains("N")) {
        direction_string = "N";
        this->renderer_flip = SDL_FLIP_NONE;
      }
      break;
    default:
      SDL_Log("Unhandled direction used!");
      break;
  }

  return direction_string;
}


void Animation::loadSurfaces(std::string direction_string, AnimationData * data) {
  if (data == nullptr || data->frame_count == 0) {
    SDL_Log("No frames in animation data");
    return;
  }
  this->frame_time_in_ms = data->frame_time_in_ms;
  this->has_background = data->has_background;

  assert(data != nullptr);
  assert(data->width > 0);
  assert(data->height > 0);

  // TODO: Determine offset
  this->surfaces[direction_string] = std::vector<SDL_Surface *>();
  for(int i = 0; i < ((int) data->frame_count + (int) data->has_background); i++) {
    this->surfaces[direction_string].push_back(SDL_CreateRGBSurfaceWithFormat(0, data->width, data->height, 0, SDL_PIXELFORMAT_RGBA32));
    for(int y = 0; y < data->frames[i].height; y++) {
      int x = 0;
      for(int instruction = 0; instruction < data->frames[i].lines[y].instruction_count; instruction++) {
        x += data->frames[i].lines[y].instructions[instruction].offset;
        for(int p = 0; p < data->frames[i].lines[y].instructions[instruction].color_count; p++, x++) {
          if (x < 0 || x >= data->width || y < 0 || y >= data->height) {
              SDL_Log("Failure to draw within the bounds");
              break;
            }
            if (data->frames[i].is_shadow) {
              ((uint32_t *) this->surfaces[direction_string][i]->pixels)[data->width * y + x] = 0xFF000000;
            } else {
              ((uint32_t *) this->surfaces[direction_string][i]->pixels)[data->width * y + x] = data->pallet->colors[*(uint8_t *)data];
            }
        }
      }
    }
  }
  assert(!this->surfaces[direction_string].empty());
  free(data);
}
