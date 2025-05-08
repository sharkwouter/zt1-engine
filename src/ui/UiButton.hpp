#ifndef UI_BUTTON_HPP
#define UI_BUTTON_HPP

#include <string>

#include <SDL2/SDL.h>

#include "UiElement.hpp"
#include "../IniReader.hpp"

class UiButton : UiElement {
public:
  UiButton(IniReader * ini_reader, std::string name);
  ~UiButton();

  void draw(SDL_Renderer *renderer);

private:
  SDL_Texture * button;
};

#endif // UI_BUTTON_HPP