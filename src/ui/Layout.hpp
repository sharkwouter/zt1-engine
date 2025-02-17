#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include <vector>

#include <SDL2/SDL.h>

#include "UiElement.hpp"

class Layout : UiElement {
public:
  Layout(int x, int y, int dx, int dy, int id, int layer);
  Layout(int id, int layer);
  ~Layout();

  void draw(SDL_Renderer * renderer);

  void addUiElement(UiElement * element);

private:
  SDL_Rect * dest_rect = NULL;

  std::vector<UiElement*> elements;
};

#endif // LAYOUT_HPP