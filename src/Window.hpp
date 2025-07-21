#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL2/SDL.h>
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

    void clear();
    void present();
private:
    SDL_Cursor * default_cursor = nullptr;
};

#endif // WINDOW_HPP