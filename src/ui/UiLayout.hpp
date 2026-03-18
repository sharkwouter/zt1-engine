#ifndef UI_LAYOUT_HPP
#define UI_LAYOUT_HPP

#include <vector>

#include <SDL3/SDL.h>

#include "UiElement.hpp"

#include "UiAction.hpp"

#include "../IniReader.hpp"
#include "../ResourceManager.hpp"
#include "../Input.hpp"

class UiLayout : public UiElement {
public:
  UiLayout(IniReader * ini_reader, ResourceManager * resource_manager);
  UiLayout(IniReader * ini_reader, ResourceManager * resource_manager, std::string name);
  ~UiLayout();

  UiAction handleInputs(std::vector<Input> &inputs);
  void draw(SDL_Renderer * renderer, SDL_FRect * layout_rect);

protected:
  std::string name = "";
  std::vector<UiElement*> elements;
  
  void process_sections();
  void process_layout(std::string layout);

};

#endif // UI_LAYOUT_HPP