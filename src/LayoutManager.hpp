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
  void Draw(SDL_Renderer * renderer, SDL_FRect * window_rect);

  void Load(std::atomic<float> * progress, std::atomic<bool> * is_done);

private:
  std::unordered_map<std::string, UiLayout*> layouts;
  UiLayout * current_layout;
  int id = 0;
  bool loaded = false;
  ResourceManager * resource_manager = nullptr;

  bool handleTargetlessAction(UiAction);
};

#endif // LAYOUT_MANAGER_HPP