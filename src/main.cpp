#include <SDL2/SDL.h>

#include "Config.hpp"
#include "Window.hpp"
#include "ResourceManager.hpp"
#include "IniReader.hpp"
#include "LoadScreen.hpp"

#include "ui/Layout.hpp"


int main(int argc, char *argv[]) {
  Config config;
  ResourceManager resource_manager(&config);

  Window window("ZT1-Engine", config.getScreenWidth(), config.getScreenHeight(), 60.0f);

  LoadScreen::run(&window,&config, &resource_manager);

  IniReader startup_lyt_reader = resource_manager.getIniReader("ui/startup.lyt");
  Layout startup_layout(&startup_lyt_reader, &resource_manager);

  Mix_Music * music = resource_manager.getMusic(config.getMenuMusic());
  Mix_VolumeMusic(MIX_MAX_VOLUME);
  Mix_PlayMusic(music, -1);

  SDL_Event event;
  int running = 1;
  while (running > 0) {
    window.clear();
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = 0;
          break;
      }
    }
    startup_layout.draw(window.renderer);
    window.present();
  }

  return 0;
}