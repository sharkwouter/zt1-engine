#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP

#include <vector>
#include <string>
#include <map>

#include <SDL2/SDL.h>

#include "UiAction.hpp"

#include "../IniReader.hpp"
#include "../ResourceManager.hpp"
#include "../Input.hpp"

#define FONT_SIZE 24

class UiElement {
public:
  virtual ~UiElement() {};

  virtual UiAction handleInputs(std::vector<Input> &inputs) = 0;
  virtual void draw(SDL_Renderer * renderer, SDL_Rect * layout_rect) = 0;

  std::string getName() {return this->name;};
  int getLayer() {return this->layer;};
  int getAnchor() {return this->anchor;};

  bool hasId(int id) {
    if (id == this->id) {
      return true;
    }
    for (UiElement * child : this->children) {
      if (child->hasId(id)) {
        return true;
      }
    }
    return false;
  }

  void addChild(UiElement * new_child) {
    if(new_child->anchor == this->id) {
      this->children.push_back(new_child);
      return;
    } else {
      for (UiElement * child : this->children) {
        if (child->hasId(new_child->anchor)) {
          child->addChild(new_child);
          return;
        }
      }
    }
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "This code should never be reached, which was the child not added?");
  }

protected:
  IniReader * ini_reader = nullptr;
  ResourceManager * resource_manager = nullptr;
  std::string name;
  int id = 0;
  int layer = 0;
  int anchor = 0;

  std::vector<UiElement*> children;

  void drawChildren(SDL_Renderer * renderer, SDL_Rect * parent_rect) {
    // TODO: Figure out if layers need to be taken into account here
    for (UiElement * child : this->children) {
      child->draw(renderer, parent_rect);
    }
  }

  UiAction handleInputChildren(std::vector<Input> &inputs) {
    UiAction action = UiAction::NONE;
    for (UiElement * child : this->children) {
      UiAction new_action = child->handleInputs(inputs);
      if (new_action != UiAction::NONE) {
        action = new_action;
      }
    }
    return action;
  }

  SDL_Rect getRect(std::map<std::string, std::string> map, SDL_Rect * layout_rect) {
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

    if(!map.contains("dx") || map["dx"] == "whole") {
      rect.w = layout_rect->w;
    } else {
      rect.w = std::stoi(map["dx"]);
    }

    if(!map.contains("dy") || map["dy"] == "whole") {
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

    rect.x += layout_rect->x;
    rect.y += layout_rect->y;

    return rect;
  };
};

#endif // UI_ELEMENT_HPP