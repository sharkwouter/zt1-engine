#ifndef UI_SCROLLBAR_HPP
#define UI_SCROLLBAR_HPP

#include <string>

#include <SDL3/SDL.h>

#include "UiElement.hpp"
#include "../IniReader.hpp"
#include "../Animation.hpp"
#include "../ResourceManager.hpp"
#include "../CompassDirection.hpp"

class UiScrollBar : public UiElement {
public:
  UiScrollBar(IniReader * ini_reader, ResourceManager * resource_manager, std::string name, UiElement * scrolling_element);
  ~UiScrollBar();

  UiAction handleInputs(std::vector<Input> &inputs);
  void draw(SDL_Renderer * renderer, SDL_FRect * layout_rect);

private:
  UiElement * scrolling_element = nullptr;
  Animation * animation = nullptr;
  Animation * upArrow = nullptr;
  Animation * downArrow = nullptr;
  Animation * thumb = nullptr;

  void generateDrawRect(std::map<std::string, std::string> map, SDL_FRect * layout_rect);

protected:
  SDL_FRect draw_rect = {0.0f, 0.0f, 0.0f, 0.0f};
};

#endif // UI_SCROLLBAR_HPP