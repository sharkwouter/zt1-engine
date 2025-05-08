#ifndef UI_BUTTON_HPP
#define UI_BUTTON_HPP

#include <string>

#include <SDL2/SDL.h>

#include "UiElement.hpp"
#include "../IniReader.hpp"
#include "../ResourceManager.hpp"

class UiButton : UiElement {
public:
  UiButton(IniReader * ini_reader, ResourceManager * resource_manager, std::string name);
  ~UiButton();

  void draw(SDL_Renderer * renderer, SDL_Rect * layout_rect);

private:
  std::string text_string = "";
  SDL_Texture * text = nullptr;
};

#endif // UI_BUTTON_HPP