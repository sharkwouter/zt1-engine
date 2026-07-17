#ifndef UI_LISTBOX_HPP
#define UI_LISTBOX_HPP

#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "UiElement.hpp"
#include "../IniReader.hpp"
#include "../Animation.hpp"
#include "../ResourceManager.hpp"
#include "../CompassDirection.hpp"

class UiListBox : public UiElement {
public:
  UiListBox(IniReader * ini_reader, ResourceManager * resource_manager, std::string name);
  ~UiListBox();

  UiAction handleInputs(std::vector<Input> &inputs);
  void draw(SDL_Renderer * renderer, SDL_FRect * layout_rect);

private:
  Animation * animation = nullptr;
};

#endif // UI_LISTBOX_HPP