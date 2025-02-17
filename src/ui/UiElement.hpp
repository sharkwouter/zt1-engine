#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP

#include <SDL2/SDL.h>

class UiElement {
public:
  virtual void draw(SDL_Renderer *renderer) = 0;
  int state = 1;
};

#endif // UI_ELEMENT_HPP