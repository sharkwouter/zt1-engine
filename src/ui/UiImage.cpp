#include "UiImage.hpp"

UiImage::UiImage(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);

  std::string normal = ini_reader->get(name, "normal");
  if (!normal.empty()) {
    this->image_path = normal;
  }
  if(ini_reader->isList(name, "animation")) {
    std::vector<std::string> animation_list = ini_reader->getList(name, "animation");
    this->image_path = animation_list[animation_list.size() - 1];
  } else {
    this->image_path = ini_reader->get(name, "animation");
  }
}

UiImage::~UiImage() {
  SDL_DestroyTexture(this->image);
}

void UiImage::draw(SDL_Renderer *renderer, SDL_Rect * layout_rect) {
  if (this->image_path.empty()) {
    return;
  }
  if (!this->image) {
    this->image = this->resource_manager->getTexture(renderer, this->image_path);
  }
  SDL_Rect dest_rect = this->getRect(this->ini_reader->getSection(this->name), layout_rect);
  SDL_RenderCopy(renderer, this->image, NULL, &dest_rect);
}
