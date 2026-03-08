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

  virtual UiAction handleInputs(std::vector<Input> &inputs) {
    UiAction result = {Action::NONE, 0, 0};

    // Go through layers from high to low
    for (int l=8; l > (0 - 1); l--) {
      // Process event in children where the current layer matches first
      for (UiElement * child : this->children) {
        if (child == NULL || child->getLayer() != l || !child->getActive()) {
          continue;
        }
        result = child->handleInputs(inputs);
        if (result.source != 0) {
          SDL_Log("Got event from %s (%i) at layer %i: action=%i, target=%i, source=%i", child->getName().c_str(), child->getId(), child->getLayer(), (int) result.action, result.target, result.source);
          return result;
        } else if (result.action != Action::NONE) {
          SDL_Log("Got event without source from %i", child->getId());
        }
      }
      // Process event in self if the layer matches
      if (l == this->layer) {
        for (Input input : inputs) {
          if (input.type != InputType::POSITIONED) {
            continue;
          }
          if (input.position.x < this->draw_rect.x || input.position.x > this->draw_rect.x + this->draw_rect.w) {
            continue;
          }
          if (input.position.y < this->draw_rect.y || input.position.y > this->draw_rect.y + this->draw_rect.h) {
            continue;
          }
          switch (input.event) {
            case InputEvent::LEFT_CLICK:
              result = {this->action, this->target, this->id};
              break;
            default:
              break;
          }
          if (result.source != 0) {
            return result;
          }
        }
      }
    }

    // Return our sad empty result
    return result;
  }

  virtual void draw(SDL_Renderer * renderer, SDL_Rect * layout_rect) = 0;

  std::string getName() {return this->name;};
  int getLayer() {return this->layer;};
  int getAnchor() {return this->anchor;};

  int getId() {return id;};
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

  bool getActive() {return this->active;};
  void setActive(bool active) {this->active = active;};

protected:
  IniReader * ini_reader = nullptr;
  ResourceManager * resource_manager = nullptr;
  std::string name;
  int id = 0;
  int layer = 0;
  int anchor = 0;
  int target = 0;
  bool active = true;
  Action action = Action::NONE;
  SDL_Rect draw_rect = {0, 0, 0, 0};

  std::vector<UiElement*> children;

  void drawChildren(SDL_Renderer * renderer, SDL_Rect * parent_rect) {
    for (int l=0; l < (8 + 1); l++) {
      for (UiElement * child : this->children) {
        if (child->layer == l) {
          child->draw(renderer, parent_rect);
        }
      }
    }
  }

  UiAction handleInputChildren(std::vector<Input> &inputs) {
    for (int l=8; l > (0 - 1); l--) {
      for (UiElement * child : this->children) {
        if (child == NULL || child->getLayer() != l || !child->getActive()) {
          continue;
        }
        UiAction result = child->handleInputs(inputs);
        if (result.source != 0) {
          SDL_Log("Got event from %s (%i) at layer %i: action=%i, target=%i, source=%i", child->getName().c_str(), child->getId(), child->getLayer(), (int) result.action, result.target, result.source);
          return result;
        } else if (result.action != Action::NONE) {
          SDL_Log("Got event without source from %i", child->getId());
        }
      }
    }
    return {Action::NONE, 0, 0};
  }

  void getDrawRect(std::map<std::string, std::string> map, SDL_Rect * layout_rect) {
    SDL_Rect rect = {0, 0, 0, 0};

    if (map.contains("dx")) {
      if (map["dx"] == "whole") {
        rect.w = layout_rect->w;
      } else {
        rect.w = std::stoi(map["dx"]);
      }
    }

    if (map.contains("dy") && map["dy"] != "fitfont") {
       if (map["dy"] == "whole") {
        rect.h = layout_rect->h;
      } else {
        rect.h = std::stoi(map["dy"]);
      }
    }

    if (map["x"] == "center") {
      rect.x = layout_rect->w / 2 - rect.w / 2;
    } else if (map["x"] == "right") {
      rect.x = layout_rect->w - rect.w;
    } else if (map["x"].empty() || map["x"] == "left") {
      rect.x = 0;
    } else {
      rect.x = std::stoi(map["x"]);
    }

    if (map["y"] == "center") {
      rect.y = layout_rect->h / 2 - rect.h / 2;
    } else if (map["y"] == "bottom") {
      rect.y = layout_rect->h - rect.h;
    } else if (map["y"].empty() || map["y"] == "top") {
      rect.y = 0;
    } else {
      rect.y = std::stoi(map["y"]);
    }

    if (map.contains("justify")) {
      if (map["justify"] == "center") {
        rect.x += rect.w / 2;
      } else if (map["justify"] == "right") {
        rect.x += rect.w;
      }
    }

    rect.x += layout_rect->x;
    rect.y += layout_rect->y;

    this->draw_rect = rect;
  };
};

#endif // UI_ELEMENT_HPP