#include <SDL2/SDL.h>

#include "Config.hpp"
#include "Window.hpp"
#include "ResourceManager.hpp"
#include "IniReader.hpp"
#include "LoadScreen.hpp"
#include "InputManager.hpp"
#include "Input.hpp"
#include "AniFile.hpp"

#include "ui/UiLayout.hpp"


int main(int argc, char *argv[]) {
  Config config;
  ResourceManager resource_manager(&config);

  Window window("ZT1-Engine", config.getScreenWidth(), config.getScreenHeight(), 60.0f);

  LoadScreen::run(&window, &config, &resource_manager);

  IniReader * lyt_reader = resource_manager.getIniReader("ui/startup.lyt");
  UiLayout * layout = new UiLayout(lyt_reader, &resource_manager);

  Mix_Music * music = resource_manager.getMusic(config.getMenuMusic());
  Mix_VolumeMusic(MIX_MAX_VOLUME);
  Mix_PlayMusic(music, -1);

  InputManager input_manager;
  std::vector<Input> inputs;

  int running = 1;
  UiAction action = UiAction::NONE;
  while (running > 0) {
    window.clear();
    inputs = input_manager.getInputs();
    for (Input input : inputs) {
      if (input.event == InputEvent::QUIT) {
        running = 0;
      }
    }
  
    action = layout->handleInputs(inputs);
    switch (action) {
      case UiAction::STARTUP_EXIT:
        running = false;
        break;
      case UiAction::STARTUP_CREDITS:
        free(layout);
        lyt_reader = resource_manager.getIniReader("ui/credits.lyt");
        layout = new UiLayout(lyt_reader, &resource_manager);
        break;
      case UiAction::STARTUP_PLAY_FREEFORM:
        free(layout);
        lyt_reader = resource_manager.getIniReader("ui/mapselec.lyt");
        layout = new UiLayout(lyt_reader, &resource_manager);
        break;
      case UiAction::STARTUP_PLAY_SCENARIO:
        free(layout);
        lyt_reader = resource_manager.getIniReader("ui/scenario.lyt");
        layout = new UiLayout(lyt_reader, &resource_manager);
        break;
      case UiAction::STARTUP_ZOO_ITEMS:
        free(layout);
        lyt_reader = resource_manager.getIniReader("ui/gameopts.lyt");
        layout = new UiLayout(lyt_reader, &resource_manager);
        break;
      case UiAction::SCENARIO_BACK_TO_MAIN_MENU:
        free(layout);
        lyt_reader = resource_manager.getIniReader("ui/startup.lyt");
        layout = new UiLayout(lyt_reader, &resource_manager);
        break;
      default:
        if (action != UiAction::NONE) {
          SDL_Log("Got unassigned action %i", (int) action);
        }
        break;
    }
    layout->draw(window.renderer, NULL);

    window.present();
  }

  return 0;
}