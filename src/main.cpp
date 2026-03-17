#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Config.hpp"
#include "Window.hpp"
#include "ResourceManager.hpp"
#include "LoadScreen.hpp"
#include "InputManager.hpp"
#include "Input.hpp"

#include "LayoutManager.hpp"


int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  Config config;
  ResourceManager resource_manager(&config);
  LayoutManager layout_manager(&resource_manager);
  InputManager input_manager;
  std::vector<Input> inputs;

  Window window("ZT1-Engine", config.getScreenWidth(), config.getScreenHeight(), 60.0f);
  window.set_cursor(resource_manager.getCursor(9));

  LoadScreen::run(&window, &config, &resource_manager, &layout_manager);

  int running = 1;
  while (running > 0) {
    window.clear();
    inputs = input_manager.getInputs();
    for (Input input : inputs) {
      if (input.event == InputEvent::QUIT) {
        running = 0;
      }
    }
    if (running) {
      running = layout_manager.HandleInputs(inputs);
      layout_manager.Draw(window.renderer, window.getWindowRect());

      window.present();
    }
  }

  return 0;
}