#include "LoadScreen.hpp"

#include <SDL2/SDL.h>

#include <thread>
#include <atomic>

void LoadScreen::run(Window * window, Config * config, ResourceManager * resource_manager) {
  SDL_Texture * background = resource_manager->getLoadTexture(window->renderer);
  
  SDL_Event event;

  std::atomic<float> progress;
  std::atomic<bool> is_done = false;
  SDL_Color loading_bar_color = config->getProgressColor();
  SDL_Rect background_rect;
  SDL_QueryTexture(background, NULL, NULL, &background_rect.w, &background_rect.h);
  SDL_Rect window_rect;
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
    SDL_GetWindowSize(window->window, &window_rect.w, &window_rect.h);
    background_rect.x = window_rect.w /2 - background_rect.w / 2;
    background_rect.y = window_rect.h /2 - background_rect.h / 2;
    SDL_RenderCopy(window->renderer, background, NULL, &background_rect);

    SDL_Rect loading_bar_draw_rect = loading_bar_rect;
    loading_bar_draw_rect.x += background_rect.x;
    loading_bar_draw_rect.y += background_rect.y;
    loading_bar_draw_rect.w = (int) ((float) loading_bar_draw_rect.w / 100.0f * (float) progress);
    SDL_SetRenderDrawColor(window->renderer, loading_bar_color.r, loading_bar_color.g, loading_bar_color.b, 255);
    SDL_RenderFillRect(window->renderer, &loading_bar_draw_rect);

    window->present();
  }
  SDL_DestroyTexture(background);
  loading_thread.join();
}
