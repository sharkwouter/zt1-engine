#include "Window.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// #define INTERNAL_WIDTH 800
// #define INTERNAL_HEIGHT 600

Window::Window(const std::string &title, int width, int height, float fps_target) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER) != 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't init SDL: %s", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() == -1) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't init SDL_ttf: %s", TTF_GetError());
        exit(2);
    }

    if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1 ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't init SDL_mixer: %s", Mix_GetError());
        exit(3);
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME);

    this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
    if (this->window == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't create window: %s", SDL_GetError());
        exit(4);
    }

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (this->renderer == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't create renderer: %s", SDL_GetError());
        exit(5);
    }

    // Make the use of transparency possible
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);

    this->start_frame = SDL_GetTicks();
    this->frame_delay = 1000.0 / fps_target;
}

void Window::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);
}

void Window::present() {
    int delta = SDL_GetTicks() - this->start_frame;
    if (delta < this->frame_delay) {
        SDL_Delay(this->frame_delay - delta);
    }
    SDL_RenderPresent(this->renderer);
    this->start_frame = SDL_GetTicks();
}

Window::~Window() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_FreeCursor(this->default_cursor);
    Mix_Quit();
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}