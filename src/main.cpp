#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <strings.h>
#include <unordered_map>
#include <filesystem>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Config.hpp"
#include "Window.hpp"
#include "ResourceManager.hpp"
#include "ZtdFile.hpp"
#include "IniReader.hpp"



int main(int argc, char *argv[]) {
  Config config;
  ResourceManager resource_manager(&config);
  Window window("ZT1-Engine", 800, 600, 60.0f);

  resource_manager.load();
  IniReader startup_lyt_reader = resource_manager.getIniReader("ui/startup.lyt");
  std::string startup_background = startup_lyt_reader.get("background", "animation");
  if (startup_background.empty()) {
    startup_background = startup_lyt_reader.get("background", "normal");
  }
  SDL_Texture * background = resource_manager.getTexture(window.renderer, startup_background);
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
    SDL_RenderCopy(window.renderer, background, NULL, NULL);
    window.present();
  }

  SDL_DestroyTexture(background);

  return 0;
}