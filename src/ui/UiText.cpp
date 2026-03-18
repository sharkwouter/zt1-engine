#include "UiText.hpp"

UiText::UiText(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);
  if (ini_reader->getInt(name, "anchor", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor"));
  }
  if (ini_reader->getInt(name, "anchor1", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor1"));
  }
  if (ini_reader->getInt(name, "anchor2", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor2"));
  }
  this->font = ini_reader->getInt(name, "font");
  uint32_t string_id = (uint32_t) ini_reader->getUnsignedInt(name, "id");
  this->text_string = this->resource_manager->getString(string_id);
  
  if(this->text_string.empty()) {
    if (this->id == 7119) {
      this->text_string = "Version Number: ZT1-Engine 0.1  ";
    } else {
      this->text_string = "Not found";
    }
  }
}

UiText::~UiText() {
  SDL_DestroyTexture(text);
  for (UiElement * child : this->children) {
    free(child);
  }
}

void UiText::draw(SDL_Renderer * renderer, SDL_FRect * layout_rect) {
  if (!this->text_string.empty() && (!this->text || !this->shadow)) {
    std::vector<std::string> color_values = ini_reader->getList(name, "forecolor");
    SDL_Color color = {
      (uint8_t) std::stoi(color_values[0]),
      (uint8_t) std::stoi(color_values[1]),
      (uint8_t) std::stoi(color_values[2]),
      255,
    };
    this->text = this->resource_manager->getStringTexture(renderer, this->font, this->text_string, color);
    this->shadow = this->resource_manager->getStringTexture(renderer, this->font, this->text_string,  {0, 0, 0, 255});
  }
  this->generateDrawRect(this->ini_reader->getSection(this->name), layout_rect);
  SDL_FRect text_rect = {draw_rect.x, draw_rect.y, 0.0f, 0.0f};
  SDL_GetTextureSize(this->text, &text_rect.w, &text_rect.h);
  if (this->ini_reader->get(this->name, "justify") == "center") {
    text_rect.x = draw_rect.x + (draw_rect.w / 2.0f) - (text_rect.w / 2.0f);
    text_rect.y = draw_rect.y + (draw_rect.h / 2.0f) - (text_rect.h / 2.0f);
  } else if (this->ini_reader->get(this->name, "justify") == "right") {
    text_rect.x = draw_rect.x + draw_rect.w - text_rect.w;
  }

  // Fix for version text on main menu
  if (this->ini_reader->get(this->name, "dy") == "fitfont" && this->ini_reader->get(this->name, "y") == "bottom") {
    text_rect.y -= text_rect.h;
  }
  
  shadow_rect = {text_rect.x - 1.0f, text_rect.y + 1.0f, text_rect.w, text_rect.h};
  SDL_RenderTexture(renderer, this->shadow, NULL, &shadow_rect);

  SDL_RenderTexture(renderer, this->text, NULL, &text_rect);
  this->drawChildren(renderer, &draw_rect);
}
