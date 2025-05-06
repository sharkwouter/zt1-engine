#include "LoadScreen.hpp"

#include <SDL2/SDL.h>

#include <thread>
#include <atomic>

void LoadScreen::run(Window * window, Config * config, ResourceManager * resource_manager) {
  SDL_Texture * background = resource_manager->getLoadTexture(window->renderer, config->getLangDllName());
  
  SDL_Event event;

  std::atomic<int> progress;
  std::atomic<bool> is_done = false;
  SDL_Color loading_bar_color = config->getProgressColor();
  SDL_Rect loading_bar_rect = config->getProgressPosition();

  std::thread loading_thread(&ResourceManager::load_all, resource_manager, &progress, &is_done);
  while (is_done == false) {
    window->clear();
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          std::exit(0);
          break;
      }
    }
    SDL_RenderCopy(window->renderer, background, NULL, NULL);

    SDL_Rect loading_bar_draw_rect = loading_bar_rect;
    loading_bar_draw_rect.w = (int) ((float) loading_bar_draw_rect.w / 100.0f * (float) progress);
    SDL_SetRenderDrawColor(window->renderer, loading_bar_color.r, loading_bar_color.g, loading_bar_color.b, 255);
    SDL_RenderFillRect(window->renderer, &loading_bar_draw_rect);

    window->present();
  }
  SDL_DestroyTexture(background);
  loading_thread.join();
}
