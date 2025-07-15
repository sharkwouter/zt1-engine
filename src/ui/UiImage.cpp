#include "UiImage.hpp"

#include "../Utils.hpp"
#include "../CompassDirection.hpp"

UiImage::UiImage(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);
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

UiAction UiImage::handleInputs(std::vector<Input> &inputs) {
  return handleInputChildren(inputs);
}

void UiImage::draw(SDL_Renderer *renderer, SDL_Rect * layout_rect) {
  if (!this->image && !this->animation && !this->image_path.empty()) {
    std::string extension = Utils::getFileExtension(this->image_path);
    if(extension.empty() || extension == "ANI") {
      this->animation = this->resource_manager->getAniFile(this->image_path);
    } else {
      this->image = this->resource_manager->getTexture(renderer, this->image_path);
    }
  }

  SDL_Rect dest_rect = this->getRect(this->ini_reader->getSection(this->name), layout_rect);
  if (this->image) {
    SDL_RenderCopy(renderer, this->image, NULL, &dest_rect);
  }
  if (this->animation) {
    this->animation->draw(renderer, dest_rect.x, dest_rect.y, CompassDirection::N);
  }
  this->drawChildren(renderer, &dest_rect);
}
