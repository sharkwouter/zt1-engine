#ifndef UI_IMAGE_HPP
#define UI_IMAGE_HPP

#include <string>

#include <SDL2/SDL.h>

#include "UiElement.hpp"
#include "../IniReader.hpp"
#include "../ResourceManager.hpp"
#include "../AniFile.hpp"

class UiImage : UiElement {
public:
  UiImage(IniReader * ini_reader, ResourceManager * resource_manager, std::string name);
  ~UiImage();

  UiAction handleInputs(std::vector<Input> &inputs);
  void draw(SDL_Renderer *renderer, SDL_Rect * layout_rect);

private:
  std::string image_path = "";
  SDL_Texture * image = nullptr;
  AniFile * animation = nullptr;
};

#endif // UI_IMAGE_HPP