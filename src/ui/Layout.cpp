#include "Layout.hpp"

#include "UiImage.hpp"
#include "UiText.hpp"
#include "UiButton.hpp"

Layout::Layout(IniReader * ini_reader, ResourceManager * resource_manager) {
  this->id = ini_reader->getInt(name, "id", 0);
  this->layer_count = ini_reader->getInt(name, "layer", 0);

  for(std::string section: ini_reader->getSections()) {
    if (section == this->name) {
      continue;
    }
    std::string element_type = ini_reader->get(section, "type");
    if (element_type == "UIImage") {
      this->elements.push_back((UiElement *) new UiImage(ini_reader, resource_manager, section));
      continue;
    }
    if (element_type == "UIButton") {
      this->elements.push_back((UiElement *) new UiButton(ini_reader, resource_manager, section));
      continue;
    }
    if (element_type == "UIText") {
      this->elements.push_back((UiElement *) new UiText(ini_reader, resource_manager, section));
      continue;
    }
  }
}

Layout::~Layout() {
    for (UiElement * element : this->elements) {
      free(element);
    }
}

void Layout::draw(SDL_Renderer *renderer) {
  if (!window) {
    this->window = SDL_RenderGetWindow(renderer);
  }
  SDL_GetWindowSize(this->window, &this->layout_rect.w, &this->layout_rect.h);
  for(int layer = 1; layer < this->layer_count + 2; layer++) {
    for (UiElement * element : this->elements) {
      if (element->getLayer() != layer) {
        continue;
      }
      element->draw(renderer, &this->layout_rect);
    }
  }
}
