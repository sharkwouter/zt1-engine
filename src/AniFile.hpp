#ifndef ANI_FILE_HPP
#define ANI_FILE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "IniReader.hpp"
#include "CompassDirection.hpp"
#include "PalletManager.hpp"
#include "Pallet.hpp"

typedef struct {
    uint8_t offset;
    uint8_t color_count;
    uint8_t * colors;
} AnimationFrameDrawInstruction;

typedef struct {
    uint8_t instruction_count;
    AnimationFrameDrawInstruction * instructions;
} AnimationLineInfo;

typedef struct __attribute__((packed)) {
    uint32_t size;
    uint16_t height;
    uint16_t width;
    int16_t offset_y;
    int16_t offset_x;
    uint16_t mystery_bytes;
} AnimationFrameMetaData;

typedef struct {
    AnimationFrameMetaData metadata;
    bool is_shadow;
    AnimationLineInfo * lines;
} AnimationFrameInfo;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t has_background;
    uint32_t frame_time_in_ms;
    Pallet * pallet;
    uint32_t frame_count;
    AnimationFrameInfo * frames;
} AnimationData;

class AniFile {
public:
    AniFile(PalletManager * pallet_manager, const std::string &ztd_file, const std::string &file_name);
    ~AniFile();

    void draw(SDL_Renderer * renderer, int x, int y, CompassDirection direction=CompassDirection::N);
    void draw(SDL_Renderer * renderer, SDL_Rect * draw_rect, CompassDirection direction=CompassDirection::N);

    AnimationData * loadAnimationData(PalletManager * pallet_manager, const std::string &ztd_file, const std::string &directory);
private:
    int current_frame = 0;
    CompassDirection last_direction = CompassDirection::N;
    SDL_RendererFlip renderer_flip = SDL_FLIP_NONE;
    uint32_t frame_start_time = 0;

    int width;
    int height;

    int x0;
    int y0;

    uint32_t pallet_color_count = 0;
    uint32_t * pallet_colors = nullptr;

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

    std::unordered_map<std::string, AnimationData *> animations;

    std::string getAnimationDirectory(IniReader * ini_reader);
    std::string convertCompassDirectionToExistingAnimationString(CompassDirection direction);
    Animation loadAnimation(const std::string &ztd_file, const std::string &directory);
};

#endif // ANI_FILE_HPP