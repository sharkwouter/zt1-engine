#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include <vector>

#include <SDL2/SDL.h>

#include "UiElement.hpp"
#include "../IniReader.hpp"

class Layout : UiElement {
public:
  Layout(IniReader * ini_reader);
  ~Layout();

  void draw(SDL_Renderer * renderer);

private:
  SDL_Rect dest_rect;

  std::vector<UiElement*> elements;
};

#endif // LAYOUT_HPP