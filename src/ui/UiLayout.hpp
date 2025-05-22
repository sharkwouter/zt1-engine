#ifndef UI_LAYOUT_HPP
#define UI_LAYOUT_HPP

#include <vector>

#include <SDL2/SDL.h>

#include "UiElement.hpp"

#include "UiAction.hpp"

#include "../IniReader.hpp"
#include "../ResourceManager.hpp"
#include "../Input.hpp"

class UiLayout : UiElement {
public:
  UiLayout(IniReader * ini_reader, ResourceManager * resource_manager);
  UiLayout(IniReader * ini_reader, ResourceManager * resource_manager, std::string name);
  ~UiLayout();

  UiAction handleInputs(std::vector<Input> &inputs);
  void draw(SDL_Renderer * renderer, SDL_Rect * layout_rect);

private:
  std::string name = "layoutinfo";
  int id = 0;
  int layer_count = 0;
  SDL_Window * window = nullptr;

  std::vector<UiElement*> elements;
  
  void process_sections(IniReader * ini_reader, ResourceManager * resource_manager);
  void process_layout(ResourceManager * resource_manager, std::string layout);

};

#endif // UI_LAYOUT_HPP