#include "UiImage.hpp"

#include "../Utils.hpp"
#include "../CompassDirection.hpp"

UiImage::UiImage(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);
  SDL_Log("Image %s (%i) is on layer %i", name.c_str(), this->id, this->layer);

  this->anchor = ini_reader->getInt(name, "anchor", 0);

  std::string normal = ini_reader->get(name, "normal");
  if (!normal.empty()) {
    this->image_path = normal;
  } else {
    if(ini_reader->isList(name, "animation")) {
      SDL_Log("Loading animation");
      std::vector<std::string> animation_list = ini_reader->getList(name, "animation");
      this->image_path = animation_list[animation_list.size() - 1];
    } else {
      this->image_path = ini_reader->get(name, "animation");
    }
  }
}

UiImage::~UiImage() {
  if (this->image) {
    SDL_DestroyTexture(this->image);
  }
  if (this->animation) {
    free(animation);
  }
  for (UiElement * child : this->children) {
    free(child);
  }
}

void UiImage::draw(SDL_Renderer *renderer, SDL_Rect *layout_rect) {
  if (!this->image && !this->animation && !this->image_path.empty()) {
    std::string extension = Utils::getFileExtension(this->image_path);
    if (extension.empty() || extension == "ANI")
    {
      this->animation = this->resource_manager->getAnimation(this->image_path);
    } else {
    this->image = this->resource_manager->getTexture(renderer, this->image_path);
  }
}
this->getDrawRect(this->ini_reader->getSection(this->name), layout_rect);
// if (this->ini_reader->get(this->name, "justify") == "center") {
//   draw_rect.x -= draw_rect.w / 2;
// } else if (this->ini_reader->get(this->name, "justify") == "right") {
//   draw_rect.x -= draw_rect.w;
// }
if (this->image) {
  if (draw_rect.w == 0 || draw_rect.h == 0) {
    SDL_QueryTexture(this->image, NULL, NULL, &draw_rect.w, &draw_rect.h);
  }
  if (this->ini_reader->get(this->name, "y") == "bottom") {
    draw_rect.y -= draw_rect.h;
  }
  SDL_RenderCopy(renderer, this->image, NULL, &draw_rect);
}
if (this->animation) {
  if (draw_rect.w == 0 || draw_rect.h == 0) {
    this->animation->queryTexture(CompassDirection::N, &draw_rect.w, &draw_rect.h);
  }
  if (this->ini_reader->get(this->name, "y") == "bottom") {
    draw_rect.y -= draw_rect.h;
  }
  this->animation->draw(renderer, &draw_rect, CompassDirection::N);
}
this->drawChildren(renderer, &draw_rect);
}
