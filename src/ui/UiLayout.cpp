#include "UiLayout.hpp"

#include "UiImage.hpp"
#include "UiText.hpp"
#include "UiButton.hpp"

UiLayout::UiLayout(IniReader * ini_reader, ResourceManager * resource_manager) {
  this->name = "layoutinfo";
  this->id = ini_reader->getInt(this->name, "id", 0);
  this->layer = ini_reader->getInt(this->name, "layer", 1);

  this->process_sections(ini_reader, resource_manager);
}

UiLayout::UiLayout(IniReader *ini_reader, ResourceManager *resource_manager, std::string name) {
  this->name = name;
  this->id = ini_reader->getInt(name, "id", 0);
  this->layer = ini_reader->getInt(name, "layer", 1);
  this->anchor = ini_reader->getInt(name, "anchor", 0);

  this->process_layout(resource_manager, ini_reader->get(name, "layout"));
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
  this->id = ini_reader->getInt(name, "id", 0);
  this->layer_count = ini_reader->getInt(name, "layer", 0);

  for(std::string section: ini_reader->getSections()) {
    if (section == this->name || section == "layoutinfo") {
      continue;
    }

    UiElement * new_element = nullptr;
    std::string element_type = ini_reader->get(section, "type");
    if (element_type.empty()) {
      SDL_Log("Could not determine type of section %s", section.c_str());
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
            SDL_Log("id was not found");
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
  IniReader * ini_reader = resource_manager->getIniReader(layout);
  process_sections(ini_reader, resource_manager);
}

UiAction UiLayout::handleInputs(std::vector<Input> &inputs) {
  return handleInputChildren(inputs);
}
