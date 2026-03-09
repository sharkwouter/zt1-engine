#include "UiLayout.hpp"

#include <algorithm>

#include "UiImage.hpp"
#include "UiText.hpp"
#include "UiButton.hpp"

UiLayout::UiLayout(IniReader * ini_reader, ResourceManager * resource_manager) {
  this->process_sections(ini_reader, resource_manager);
}

UiLayout::UiLayout(IniReader *ini_reader, ResourceManager *resource_manager, std::string name) {
  this->name = name;
  this->active = ini_reader->getInt(this->name, "state", 0) != 1;
  this->process_layout(resource_manager, ini_reader->get(this->name, "layout"));
}

UiLayout::~UiLayout() {
    for (UiElement * element : this->children) {
      free(element);
    }
    if (ini_reader != nullptr) {
      free(ini_reader);
    }
}

void UiLayout::draw(SDL_Renderer *renderer, SDL_Rect * layout_rect) {
  if (!layout_rect) {
    if (!window) {
      this->window = SDL_RenderGetWindow(renderer);
    }
    SDL_Rect window_rect = {0, 0, 0, 0};
    SDL_GetWindowSize(this->window, &window_rect.w, &window_rect.h);
    layout_rect = &window_rect;
  }
  drawChildren(renderer, layout_rect);
}

void UiLayout::process_sections(IniReader *ini_reader, ResourceManager *resource_manager) {
  this->id = ini_reader->getInt("layoutinfo", "id", 0);
  this->layer = ini_reader->getInt("layoutinfo", "layer", 0);

  for(std::string section: ini_reader->getSections()) {
    if (section == "layoutinfo") {
      continue;
    }

    UiElement * new_element = nullptr;
    std::string element_type = ini_reader->get(section, "type");
    if (element_type.empty()) {
      SDL_Log("Could not determine type of section %s in layout %s", section.c_str(), this->name.c_str());
    }
    if (element_type == "UIImage") {
      new_element = (UiElement *) new UiImage(ini_reader, resource_manager, section);
    } else if (element_type == "UIButton") {
      new_element = (UiElement *) new UiButton(ini_reader, resource_manager, section);
    } else if (element_type == "UIText") {
      new_element = (UiElement *) new UiText(ini_reader, resource_manager, section);
    } else if (element_type == "UILayout") {
      new_element = (UiElement *) new UiLayout(ini_reader, resource_manager, section);
    }

    if (new_element) {
      if (!new_element->getAnchor() || new_element->getAnchor() == 0 || new_element->getAnchor() == this->id) {
        this->children.push_back(new_element);
      } else {
        for(UiElement * element : children) {
          if (element->hasId(new_element->getAnchor())) {
            element->addChild(new_element);
          } else {
            SDL_Log("The id %i was not found while trying to attach %i to it", new_element->getAnchor(), new_element->getId());
          }
        }
      }
    }
  }
}

void UiLayout::process_layout(ResourceManager *resource_manager, std::string layout) {
  if (layout.empty()) {
    return;
  }
  if (layout != "ui/infocomC.lyt") {
    // infocomC.lyt is the only lyt file that has a capital letter in the name
    std::transform(layout.begin(), layout.end(), layout.begin(), ::tolower);
  }
  IniReader * ini_reader = resource_manager->getIniReader(layout);
  process_sections(ini_reader, resource_manager);
}

UiAction UiLayout::handleInputs(std::vector<Input> &inputs) {
  return handleInputChildren(inputs);
}
