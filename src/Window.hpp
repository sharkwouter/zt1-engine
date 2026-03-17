#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL3/SDL.h>
#include <string>

class Window {
public:
    SDL_Window *window;
    SDL_Renderer *renderer;

    Window(const std::string &title, int width, int height, float fps_target);
    ~Window();

    unsigned int start_frame;
    float frame_delay;

    void set_cursor(SDL_Cursor * cursor) {this->default_cursor = cursor; SDL_SetCursor(this->default_cursor);};

    SDL_FRect * getWindowRect();

    void clear();
    void present();
private:
    SDL_Cursor * default_cursor = nullptr;
    SDL_FRect window_rect = {0.0f, 0.0f, 0.0f, 0.0f};
};

#endif // WINDOW_HPP