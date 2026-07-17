#include "UiScrollBar.hpp"

#include "../CompassDirection.hpp"

// Hard code scroll bar sizes for now
#define SCROLLBAR_WIDTH 12.0f
#define SCROLLBAR_BUTTON_HEIGHT 17.0f


UiScrollBar::UiScrollBar(IniReader * ini_reader, ResourceManager * resource_manager, std::string name, UiElement * scrolling_element) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;
  this->scrolling_element = scrolling_element;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);
  this->target = ini_reader->getInt(name, "target", 0);
  this->action = (Action) ini_reader->getInt(name, "action", 0);
  if (ini_reader->getInt(name, "anchor", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor"));
  }
  if (ini_reader->getInt(name, "anchor1", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor1"));
  }
  if (ini_reader->getInt(name, "anchor2", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor2"));
  }

  std::string animation_path = ini_reader->get(name, "animation");
  if (!animation_path.empty()) {
    this->animation = resource_manager->getAnimation(animation_path);
  }
  std::string up_arrow_path = ini_reader->get(name, "upArrow");
  if (!up_arrow_path.empty()) {
    this->upArrow = resource_manager->getAnimation(up_arrow_path);
  }
  std::string down_arrow_path = ini_reader->get(name, "downArrow");
  if (!down_arrow_path.empty()) {
    this->downArrow = resource_manager->getAnimation(down_arrow_path);
  }
  std::string thumb_path = ini_reader->get(name, "thumb");
  if (!thumb_path.empty()) {
    this->thumb = resource_manager->getAnimation(thumb_path);
  }
}

UiScrollBar::~UiScrollBar() {
  for (UiElement * child : this->children) {
    if (child != nullptr) {
      delete child;
    }
  }
}

UiAction UiScrollBar::handleInputs(std::vector<Input> &inputs) {
  UiAction result = {Action::NONE, 0, 0};
  this->up_arrow_pressed = false;
  this->down_arrow_pressed = false;
  for (Input input : inputs) {
    if (input.type != InputType::POSITIONED) {
      continue;
    }
    SDL_FRect up_arrow_rect = {draw_rect.x, draw_rect.y, SCROLLBAR_WIDTH, SCROLLBAR_BUTTON_HEIGHT};
    if (
      (input.position.x >= up_arrow_rect.x && input.position.x <= up_arrow_rect.x + up_arrow_rect.w) &&
      (input.position.y >= up_arrow_rect.y && input.position.y <= up_arrow_rect.y + up_arrow_rect.h) &&
      (input.event == InputEvent::LEFT_CLICK)
    ) {
      this->up_arrow_pressed = true;
      continue;
    }
    SDL_FRect down_arrow_rect = {draw_rect.x, draw_rect.y + draw_rect.h - SCROLLBAR_BUTTON_HEIGHT, SCROLLBAR_WIDTH, SCROLLBAR_BUTTON_HEIGHT};
    if (
      (input.position.x >= down_arrow_rect.x && input.position.x <= down_arrow_rect.x + up_arrow_rect.w) &&
      (input.position.y >=down_arrow_rect.y && input.position.y <= down_arrow_rect.y + up_arrow_rect.h) &&
      (input.event == InputEvent::LEFT_CLICK)
    ) {
      this->down_arrow_pressed = true;
      continue;
    }
  }
  if (result.source == 0) {
    result = handleInputChildren(inputs);
  }
  return result;
}

void UiScrollBar::draw(SDL_Renderer * renderer, SDL_FRect * layout_rect) {
  this->generateDrawRect(this->ini_reader->getSection(this->name), layout_rect);
  if (this->animation != nullptr) {
    this->animation->draw(renderer, &draw_rect, CompassDirection::N);
  }
  if (this->upArrow != nullptr) {
    SDL_FRect up_arrow_rect = {draw_rect.x, draw_rect.y, 0.0f, 0.0f};
    if (up_arrow_pressed) {
      this->upArrow->draw(renderer, &up_arrow_rect, CompassDirection::S);
    } else {
      this->upArrow->draw(renderer, &up_arrow_rect, CompassDirection::N);
    }
  }
  if (this->downArrow != nullptr) {
    SDL_FRect down_arrow_rect = {draw_rect.x, draw_rect.y + draw_rect.h - SCROLLBAR_BUTTON_HEIGHT, 0.0f, 0.0f};
    if (down_arrow_pressed) {
      this->downArrow->draw(renderer, &down_arrow_rect, CompassDirection::S);
    } else {
      this->downArrow->draw(renderer, &down_arrow_rect, CompassDirection::N);
    }
  }
  if (this->thumb != nullptr) {
    SDL_FRect thumb_rect = {draw_rect.x, draw_rect.y + SCROLLBAR_BUTTON_HEIGHT, 0.0f, 0.0f};
    this->thumb->draw(renderer, &thumb_rect, CompassDirection::N);
  }

  this->drawChildren(renderer, &draw_rect);
}

 void UiScrollBar::generateDrawRect(std::map<std::string, std::string> map, SDL_FRect * layout_rect) {
    SDL_FRect rect = {0.0f, 0.0f, 0.0f, 0.0f};
    SDL_FRect * parent_rect = this->scrolling_element->getDrawRect();
    rect.x = parent_rect->x + parent_rect->w;
    rect.y = parent_rect->y;
    rect.w = SCROLLBAR_WIDTH;
    rect.h = parent_rect->h;

    this->draw_rect = rect;
  };
