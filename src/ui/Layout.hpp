#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include <vector>

#include <SDL2/SDL.h>

#include "UiElement.hpp"
#include "../IniReader.hpp"
#include "../ResourceManager.hpp"

class Layout {
public:
  Layout(IniReader * ini_reader, ResourceManager * resource_manager);
  ~Layout();

  void draw(SDL_Renderer * renderer);

private:
  std::string name = "layoutinfo";
  int id = 0;
  int layer_count = 0;
  SDL_Rect layout_rect;
  SDL_Window * window = nullptr;

  std::vector<UiElement*> elements;
};

#endif // LAYOUT_HPP