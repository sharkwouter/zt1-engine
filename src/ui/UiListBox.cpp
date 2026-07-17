#include "UiListBox.hpp"

#include "../CompassDirection.hpp"

UiListBox::UiListBox(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

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
  this->scrollbar = ini_reader->getInt(name, "scrollbar");

  std::string animation_path = ini_reader->get(name, "animation");
  if (!animation_path.empty()) {
    this->animation = resource_manager->getAnimation(animation_path);
  }
}

UiListBox::~UiListBox() {
  for (UiElement * child : this->children) {
    if (child != nullptr) {
      delete child;
    }
  }
}

UiAction UiListBox::handleInputs(std::vector<Input> &inputs) {
  UiAction result = {Action::NONE, 0, 0};
  for (Input input : inputs) {
    if (input.type != InputType::POSITIONED) {
      continue;
    }
  }
  if (result.source == 0) {
    result = handleInputChildren(inputs);
  }
  return result;
}

void UiListBox::draw(SDL_Renderer * renderer, SDL_FRect * layout_rect) {
  this->generateDrawRect(this->ini_reader->getSection(this->name), layout_rect);
  SDL_FRect text_rect = {draw_rect.x, draw_rect.y, 0.0f, 0.0f};
  if (this->animation != nullptr) {
    this->animation->draw(renderer, &draw_rect, CompassDirection::N);
  }

  this->drawChildren(renderer, &draw_rect);
}
