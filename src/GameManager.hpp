#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <unordered_map>
#include <vector>
#include <atomic>

#include "IniReader.hpp"
#include "ResourceManager.hpp"
#include "ui/UiLayout.hpp"
#include "ui/UiAction.hpp"

class GameManager {
public:
  GameManager(ResourceManager * resource_manager);
  ~GameManager();

  bool HandleInputs(std::vector<Input> &inputs);
  void Draw(SDL_Renderer * renderer, SDL_FRect * window_rect);

  void Load(std::atomic<float> * progress, std::atomic<bool> * is_done);

private:
  std::unordered_map<std::string, UiLayout*> layouts;
  int id = 0;
  bool loaded = false;
  ResourceManager * resource_manager = nullptr;

  bool handleTargetlessAction(UiAction);
};

#endif // GAME_MANAGER_HPP