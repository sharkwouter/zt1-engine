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

    void clear();
    void present();
};

#endif // WINDOW_HPP