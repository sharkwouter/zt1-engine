#include "UiButton.hpp"

UiButton::UiButton(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);

  uint32_t string_id = (uint32_t) ini_reader->getUnsignedInt(name, "textid");
  this->text_string = this->resource_manager->getString(string_id);
  
  if(this->text_string.empty()) {
    this->text_string = "Not found";
  }
}

UiButton::~UiButton() {
  SDL_DestroyTexture(text);
}

void UiButton::draw(SDL_Renderer * renderer, SDL_Rect * layout_rect) {
  if (!this->text) {
    std::vector<std::string> color_values = ini_reader->getList(name, "forecolor");
    SDL_Color color = {
      std::stoi(color_values[0]),
      std::stoi(color_values[1]),
      std::stoi(color_values[2]),
      255,
    };
    this->text = this->resource_manager->getStringTexture(renderer, this->text_string, color);
  }
  SDL_Rect dest_rect = this->getRect(this->ini_reader->getSection(this->name), layout_rect);
  // SDL_Rect text_rect;
  // SDL_QueryTexture(this->text, NULL, NULL, &text_rect.w, &text_rect.h);
  // dest_rect.w = std::min(dest_rect.w, text_rect.w);
  SDL_RenderCopy(renderer, this->text, NULL, &dest_rect);
}
