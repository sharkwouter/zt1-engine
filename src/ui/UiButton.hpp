#ifndef UI_BUTTON_HPP
#define UI_BUTTON_HPP

#include <SDL2/SDL.h>

class UiButton {
public:
  UiButton();
  ~UiButton();

  void draw(SDL_Renderer *renderer);
};

#endif // UI_BUTTON_HPP