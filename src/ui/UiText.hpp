#ifndef UI_TEXT_HPP
#define UI_TEXT_HPP

#include <SDL2/SDL.h>

class UiText {
public:
  UiText();
  ~UiText();

  void draw(SDL_Renderer *renderer);
};

#endif // UI_TEXT_HPP