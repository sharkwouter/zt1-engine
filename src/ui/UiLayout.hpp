#ifndef UI_LAYOUT_HPP
#define UI_LAYOUT_HPP

#include <vector>

#include <SDL2/SDL.h>

#include "UiElement.hpp"
#include "UiAction.hpp"

#include "../IniReader.hpp"
#include "../ResourceManager.hpp"
#include "../Input.hpp"

class UiLayout {
public:
  UiLayout(IniReader * ini_reader, ResourceManager * resource_manager);
  ~UiLayout();

  std::vector<UiAction> handleInputs(std::vector<Input> &inputs);
  void draw(SDL_Renderer * renderer);

private:
  std::string name = "layoutinfo";
  int id = 0;
  int layer_count = 0;
  SDL_Rect layout_rect = {0, 0, 0, 0};
  SDL_Window * window = nullptr;

  std::vector<UiElement*> elements;
};

#endif // UI_LAYOUT_HPP