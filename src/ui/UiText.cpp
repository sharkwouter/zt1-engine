#include "UiText.hpp"

UiText::UiText(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);

  uint32_t string_id = (uint32_t) ini_reader->getUnsignedInt(name, "textid");
  this->text_string = this->resource_manager->getString(string_id);
}

UiText::~UiText() {
  SDL_DestroyTexture(text);
}

void UiText::draw(SDL_Renderer * renderer, SDL_Rect * layout_rect) {
  if (!this->text) {
    // this->text = this->resource_manager->getLoadTexture(renderer, this->image_path);
  }
  SDL_Rect dest_rect = this->getRect(this->ini_reader->getSection(this->name), layout_rect);
  SDL_RenderCopy(renderer, this->text, NULL, &dest_rect);
}
