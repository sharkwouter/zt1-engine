#ifndef ANI_FILE_HPP
#define ANI_FILE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "IniReader.hpp"
#include "CompassDirection.hpp"

class AniFile {
public:
    AniFile(const std::string &ztd_file, const std::string &file_name);
    ~AniFile();

    void draw(SDL_Renderer * renderer, int x, int y, CompassDirection direction=CompassDirection::SW);
    void draw(SDL_Renderer * renderer, SDL_Rect * layout_rect, CompassDirection direction=CompassDirection::SW);

private:
    int current_frame = 0;
    CompassDirection last_direction = CompassDirection::N;
    SDL_RendererFlip renderer_flip = SDL_FLIP_NONE;
    uint32_t frame_start_time = 0;

    int width;
    int height;

    int x0;
    int y0;

    std::string file_name;

    typedef struct {
        int16_t x_offset;
        int16_t y_offset;
        uint16_t height;
        uint16_t width;
        SDL_Surface * surface;
        SDL_Texture * texture;
    } Frame;

    typedef struct {
        uint32_t frame_count;
        bool has_background;
        uint32_t frame_time_in_ms;
        std::vector<Frame> frames;
    } Animation;

    std::unordered_map<std::string, Animation> animations;

    std::string getAnimationDirectory(IniReader * ini_reader);
    std::string convertCompassDirectionToExistingAnimationString(CompassDirection direction);
    Animation loadAnimation(const std::string &ztd_file, const std::string &directory);
};

#endif // ANI_FILE_HPP