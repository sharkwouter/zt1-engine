#include "LoadScreen.hpp"

#include <SDL2/SDL.h>

#include <thread>
#include <atomic>

void LoadScreen::run(Window * window, Config * config, ResourceManager * resource_manager, LayoutManager * layout_manager) {
  SDL_Texture * background = resource_manager->getLoadTexture(window->renderer);
  
  SDL_Event event;

  std::atomic<float> resources_progress, strings_progress, layouts_progress = 0.0f;
  std::atomic<bool> resources_done, strings_done, layouts_done = false;
  SDL_Color loading_bar_color = config->getProgressColor();
  SDL_Rect background_rect;
  SDL_QueryTexture(background, NULL, NULL, &background_rect.w, &background_rect.h);
  SDL_Rect * window_rect;
  SDL_Rect loading_bar_rect = config->getProgressPosition();

  std::thread * loading_resources_thread = nullptr;;
  std::thread * loading_strings_thread = nullptr; ;
  std::thread * loading_layouts_thread = nullptr;
  while (resources_done.load() == false || strings_done.load() == false || layouts_done.load() == false) {
    window->clear();
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          std::exit(0);
          break;
      }
    }
    window_rect = window->getWindowRect();
    background_rect.x = window_rect->w /2 - background_rect.w / 2;
    background_rect.y = window_rect->h /2 - background_rect.h / 2;
    SDL_RenderCopy(window->renderer, background, NULL, &background_rect);

    float overall_progress = (resources_progress.load() + strings_progress.load() + layouts_progress.load()) / 3.0f;
    SDL_Rect loading_bar_draw_rect = loading_bar_rect;
    loading_bar_draw_rect.x += background_rect.x;
    loading_bar_draw_rect.y += background_rect.y;
    loading_bar_draw_rect.w = (int) ((float) loading_bar_draw_rect.w / 100.0f * overall_progress);
    SDL_SetRenderDrawColor(window->renderer, loading_bar_color.r, loading_bar_color.g, loading_bar_color.b, 255);
    SDL_RenderFillRect(window->renderer, &loading_bar_draw_rect);

    window->present();

    // Start loading after the first draw
    if (loading_resources_thread == nullptr) {
      loading_resources_thread = new std::thread(&ResourceManager::load_resource_map, resource_manager, &resources_progress, &resources_done);
    }
    if (loading_strings_thread == nullptr) {
      loading_strings_thread = new std::thread(&ResourceManager::load_string_map, resource_manager, &strings_progress, &strings_done);
    }
    // Only start loading layouts after the resources have been loaded
    if (resources_done.load() && loading_layouts_thread == nullptr) {
      resource_manager->PlayMenuMusic();
      loading_layouts_thread = new std::thread(&LayoutManager::Load, layout_manager, &layouts_progress, &layouts_done);
    }
  }
  SDL_DestroyTexture(background);
  loading_resources_thread->join();
  loading_strings_thread->join();
  loading_layouts_thread->join();
  free(loading_resources_thread);
  free(loading_strings_thread);
  free(loading_layouts_thread);
}
