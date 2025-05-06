#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>

#define FONT_SIZE 16
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class UiElement {
public:
  virtual void draw(SDL_Renderer *renderer) = 0;
  int state = 1;
protected:
  std::string name;
  int id;
  int layer;

  SDL_Rect getRect(std::unordered_map<std::string, std::string> map) {
    SDL_Rect rect;

    if (map["x"] == "center") {
      rect.x = SCREEN_WIDTH / 2;
     } else if (map["x"] == "right") {
        rect.x = SCREEN_WIDTH;
    } else {
      rect.x = std::stoi(map["x"]);
    }

    if (map["y"] == "center") {
      rect.y = SCREEN_HEIGHT / 2;
    } else if (map["y"] == "bottom") {
      rect.y = SCREEN_HEIGHT;

    } else {
      rect.y = std::stoi(map["x"]);
    }

    if(map["dx"] == "whole") {
      rect.x = SCREEN_WIDTH;
    } else {
      rect.w = std::stoi(map["dx"]) - rect.x;
    }

    if(map["dy"] == "whole") {
      rect.h = SCREEN_HEIGHT;
    } else if (map["dy"] == "fitfont") {
      rect.h = FONT_SIZE;
    } else {
      rect.h = std::stoi(map["dy"]) - rect.y;
    }

    if (map["justify"] == "center") {
      rect.x -= rect.w / 2;
      rect.y -= rect.h / 2;
    } else if (map["justify"] == "right") {
      rect.x -= rect.w;
      rect.y -= rect.h;
    }

    return rect;
  };
};

#endif // UI_ELEMENT_HPP