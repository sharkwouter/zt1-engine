#ifndef UI_IMAGE_HPP
#define UI_IMAGE_HPP

#include <SDL2/SDL.h>

class UiImage {
public:
  UiImage();
  ~UiImage();

  void draw(SDL_Renderer *renderer);
};

#endif // UI_IMAGE_HPP