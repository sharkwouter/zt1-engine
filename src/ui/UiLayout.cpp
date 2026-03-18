#include "UiLayout.hpp"

#include <algorithm>

#include "UiImage.hpp"
#include "UiText.hpp"
#include "UiButton.hpp"

UiLayout::UiLayout(IniReader * ini_reader, ResourceManager * resource_manager) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->process_sections();
}

UiLayout::UiLayout(IniReader *ini_reader, ResourceManager *resource_manager, std::string name) {
  this->name = name;
  this->active = ini_reader->getInt(this->name, "state", 0) != 1;
  this->resource_manager = resource_manager;
  this->process_layout(ini_reader->get(this->name, "layout"));
}

UiLayout::~UiLayout() {
    for (UiElement * element : this->children) {
      free(element);
    }
    if (this->ini_reader != nullptr) {
      free(this->ini_reader);
    }
}

void UiLayout::draw(SDL_Renderer *renderer, SDL_FRect * layout_rect) {
  this->generateDrawRect(this->ini_reader->getSection("layoutinfo"), layout_rect);
  drawChildren(renderer, &this->draw_rect);
}

void UiLayout::process_sections() {
  for(std::string section: this->ini_reader->getSections()) {
    if (section == "layoutinfo") {
      this->id = this->ini_reader->getInt(section, "id", 0);
      this->layer = this->ini_reader->getInt(section, "layer", 0);
      continue;
    }

    std::string element_type = this->ini_reader->get(section, "type");
    if (element_type.empty()) {
      SDL_Log("Could not determine type of section %s in layout %s", section.c_str(), this->name.c_str());
    } else if (element_type == "UIImage") {
      this->children.push_back((UiElement *) new UiImage(this->ini_reader, this->resource_manager, section));
    } else if (element_type == "UIButton") {
      this->children.push_back((UiElement *) new UiButton(this->ini_reader, this->resource_manager, section));
    } else if (element_type == "UIText") {
      this->children.push_back((UiElement *) new UiText(this->ini_reader, this->resource_manager, section));
    } else if (element_type == "UILayout") {
      this->children.push_back((UiElement *) new UiLayout(this->ini_reader, this->resource_manager, section));
    } else {
      SDL_Log("Support for element type %s is not yet implemented", element_type.c_str());
    }
  }
}

void UiLayout::process_layout(std::string layout) {
  if (layout.empty()) {
    return;
  }
  if (layout != "ui/infocomC.lyt") {
    // infocomC.lyt is the only lyt file that has a capital letter in the name
    std::transform(layout.begin(), layout.end(), layout.begin(), ::tolower);
  }
  this->ini_reader = this->resource_manager->getIniReader(layout);
  process_sections();
}

UiAction UiLayout::handleInputs(std::vector<Input> &inputs) {
  return handleInputChildren(inputs);
}
