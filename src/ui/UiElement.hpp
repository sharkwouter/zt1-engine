#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "../IniReader.hpp"
#include "../ResourceManager.hpp"

#define FONT_SIZE 24

class UiElement {
public:
  virtual void draw(SDL_Renderer * renderer, SDL_Rect * layout_rect) = 0;
  int getLayer() {return this->layer;};

protected:
  IniReader * ini_reader = nullptr;
  ResourceManager * resource_manager = nullptr;
  std::string name;
  int id;
  int layer;

  SDL_Rect getRect(std::unordered_map<std::string, std::string> map, SDL_Rect * layout_rect) {
    SDL_Rect rect;

    if (map["x"] == "center") {
      rect.x = layout_rect->w / 2;
     } else if (map["x"] == "right") {
        rect.x = layout_rect->w;
    } else {
      rect.x = std::stoi(map["x"]);
    }

    if (map["y"] == "center") {
      rect.y = layout_rect->h / 2;
    } else if (map["y"] == "bottom") {
      rect.y = layout_rect->h;
    } else {
      rect.y = std::stoi(map["y"]);
    }

    if(map["dx"] == "whole") {
      rect.w = layout_rect->w;
    } else {
      rect.w = std::stoi(map["dx"]);
    }

    if(map["dy"] == "whole") {
      rect.h = layout_rect->h;
    } else if (map["dy"] == "fitfont") {
      rect.h = FONT_SIZE;
    } else {
      rect.h = std::stoi(map["dy"]);
    }

    if (map["justify"] == "center" || map["x"] == "center") {
      rect.x -= rect.w / 2;
    }
    if (map["justify"] == "center" || map["y"] == "center") {
      rect.y -= rect.h / 2;
    }
    if (map["justify"] == "right") {
      rect.x -= rect.w;
      rect.y -= rect.h;
    }

    return rect;
  };
};

#endif // UI_ELEMENT_HPP