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

    UiElement * new_element = nullptr;
    std::string element_type = ini_reader->get(section, "type");
    if (element_type == "UIImage") {
      new_element = (UiElement *) new UiImage(ini_reader, resource_manager, section);
    } else if (element_type == "UIButton") {
      new_element = (UiElement *) new UiButton(ini_reader, resource_manager, section);
    } else if (element_type == "UIText") {
      new_element = (UiElement *) new UiText(ini_reader, resource_manager, section);
    }

    if (new_element) {
      if (!new_element->getAnchor() || new_element->getAnchor() == this->id) {
        this->elements.push_back(new_element);
      } else {
        for(UiElement * element : elements) {
          if (element->hasId(new_element->getAnchor())) {
            element->addChild(new_element);
          } else {
            SDL_Log("id was not found");
          }
        }
      }
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
