#ifndef LOAD_SCREEN_HPP
#define LOAD_SCREEN_HPP

#include "ResourceManager.hpp"
#include "GameManager.hpp"
#include "Config.hpp"
#include "Window.hpp"

class LoadScreen {
public:
  static void run(Window * window, Config * config, ResourceManager * resource_manager, GameManager * game_manager);
};

#endif // LOAD_SCREEN_HPP