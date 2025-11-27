#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "IniReader.hpp"
#include "CompassDirection.hpp"
#include "PalletManager.hpp"
#include "Pallet.hpp"
#include "AnimationData.hpp"

class Animation {
public:
    Animation(std::unordered_map<std::string, AnimationData *> * data);
    ~Animation();

    void draw(SDL_Renderer * renderer, int x, int y, CompassDirection direction=CompassDirection::N);
    void draw(SDL_Renderer * renderer, SDL_Rect * draw_rect, CompassDirection direction=CompassDirection::N);

    void queryTexture(CompassDirection direction, int * w, int * h);
private:
    int current_frame = 0;
    CompassDirection last_direction = CompassDirection::N;
    SDL_RendererFlip renderer_flip = SDL_FLIP_NONE;
    uint32_t frame_start_time = 0;

    uint32_t frame_time_in_ms = 0;
    bool has_background = 0;

    std::unordered_map<std::string, std::vector<SDL_Surface *>> surfaces;
    std::unordered_map<std::string, std::vector<SDL_Texture *>> textures;

    template <typename T>
    std::string convertCompassDirectionToExistingAnimationString(CompassDirection direction, std::unordered_map<std::string, T> &animation_map);
    std::string convertCompassDirectionToString(CompassDirection direction);  // TODO: Figure out if this should be here

    void loadSurfaces(std::string direction_string, AnimationData * data);
};

#endif // ANIMATION_HPP