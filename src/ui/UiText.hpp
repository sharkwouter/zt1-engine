#ifndef UI_TEXT_HPP
#define UI_TEXT_HPP

#include <string>

#include <SDL2/SDL.h>

#include "UiElement.hpp"
#include "../IniReader.hpp"
#include "../ResourceManager.hpp"

class UiText : UiElement {
public:
  UiText(IniReader * ini_reader, ResourceManager * resource_manager, std::string name);
  ~UiText();

  UiAction handleInputs(std::vector<Input> &inputs);
  void draw(SDL_Renderer * renderer, SDL_Rect * layout_rect);

private:
  std::string text_string = "";
  SDL_Texture * text = nullptr;
  SDL_Texture * shadow = nullptr;
  int font = 0;
  SDL_Rect dest_rect = {0, 0, 0, 0};
  SDL_Rect shadow_rect = {0, 0, 0, 0};
};

#endif // UI_TEXT_HPP