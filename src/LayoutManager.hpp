#ifndef LAYOUT_MANAGER_HPP
#define LAYOUT_MANAGER_HPP

#include <unordered_map>
#include <vector>
#include <atomic>

#include "IniReader.hpp"
#include "ResourceManager.hpp"
#include "ui/UiLayout.hpp"
#include "ui/UiAction.hpp"

class LayoutManager {
public:
  LayoutManager(ResourceManager * resource_manager);
  ~LayoutManager();

  bool HandleInputs(std::vector<Input> &inputs);
  void Draw(SDL_Renderer * renderer, SDL_Rect * layout_rect);

  void load_all(std::atomic<float> * progress, std::atomic<bool> * is_done);

private:
  std::unordered_map<std::string, UiLayout*> layouts;
  UiLayout * current_layout;
  int id = 0;

  void generateLayouts(IniReader * ini_reader, ResourceManager * resource_manager);

  bool handleTargetlessAction(UiAction);
};

#endif // LAYOUT_MANAGER_HPP