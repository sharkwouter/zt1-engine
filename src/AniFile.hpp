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

    void draw(SDL_Renderer * renderer, CompassDirection direction=CompassDirection::NW);

private:
    int current_frame = 0;
    CompassDirection last_direction = CompassDirection::N;
    bool draw_mirrored = false;
    uint32_t frame_start_time = 0;

    int x = 400;
    int y = 400;

    int x0;
    int y0;
    int x1;
    int y1;

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
        bool has_shadow;
        uint32_t frame_time_in_ms;
        std::vector<Frame> frames;
    } Animation;

    std::unordered_map<std::string, Animation> animations;

    std::string getAnimationDirectory(IniReader * ini_reader);
    std::string convertCompassDirectionToExistingAnimationString(CompassDirection direction);
    Animation loadAnimation(const std::string &ztd_file, const std::string &directory);
};

#endif // ANI_FILE_HPP