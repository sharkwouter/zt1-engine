#include "Window.hpp"

#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

// #define INTERNAL_WIDTH 800
// #define INTERNAL_HEIGHT 600

Window::Window(const std::string &title, int width, int height, float fps_target) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't init SDL: %s", SDL_GetError());
        exit(1);
    }

    if (!TTF_Init()) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't init SDL_ttf: %s", SDL_GetError());
        exit(2);
    }

    if(!MIX_Init()) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't init SDL_mixer: %s", SDL_GetError());
        exit(3);
    }

    this->window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
    if (this->window == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't create window: %s", SDL_GetError());
        exit(4);
    }
    this->window_rect.w = (float) width;
    this->window_rect.h = (float) height;


    this->renderer = SDL_CreateRenderer(this->window, nullptr);
    if (this->renderer == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "couldn't create renderer: %s", SDL_GetError());
        exit(5);
    }
    SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);

    // Make the use of transparency possible
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);

    this->start_frame = SDL_GetTicks();
    this->frame_delay = 1000.0 / fps_target;
}

SDL_FRect * Window::getWindowRect() {
    int width, height = 0;
    SDL_GetWindowSize(this->window, &width, &height);

    this->window_rect.w = (float) width;
    this->window_rect.h = (float) height;

    return &window_rect;
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
    SDL_DestroyCursor(this->default_cursor);
    MIX_Quit();
    SDL_Quit();
}