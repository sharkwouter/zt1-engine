#include <SDL2/SDL.h>

#include "Config.hpp"
#include "Window.hpp"
#include "ResourceManager.hpp"
#include "IniReader.hpp"
#include "LoadScreen.hpp"
#include "InputManager.hpp"
#include "Input.hpp"

#include "ui/UiLayout.hpp"


int main(int argc, char *argv[]) {
  Config config;
  ResourceManager resource_manager(&config);

  Window window("ZT1-Engine", config.getScreenWidth(), config.getScreenHeight(), 60.0f);

  LoadScreen::run(&window, &config, &resource_manager);

  IniReader * startup_lyt_reader = resource_manager.getIniReader("ui/startup.lyt");
  UiLayout startup_layout(startup_lyt_reader, &resource_manager);

  Mix_Music * music = resource_manager.getMusic(config.getMenuMusic());
  Mix_VolumeMusic(MIX_MAX_VOLUME);
  Mix_PlayMusic(music, -1);

  InputManager input_manager;
  std::vector<Input> inputs;

  int running = 1;
  while (running > 0) {
    window.clear();
    inputs = input_manager.getInputs();
    for (Input input : inputs) {
      if (input.event == InputEvent::QUIT) {
        running = 0;
      }
    }
  
    if (startup_layout.handleInputs(inputs) == UiAction::STARTUP_EXIT) {
      running = false;
    }
    startup_layout.draw(window.renderer, NULL);
    window.present();
  }

  return 0;
}