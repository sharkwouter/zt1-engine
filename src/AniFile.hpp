#ifndef ANI_FILE_HPP
#define ANI_FILE_HPP

#include <string>

#include <SDL2/SDL.h>

#include "IniReader.hpp"

class AniFile {
public:
    AniFile(const std::string &ztd_file, const std::string &file_name);
    ~AniFile();
    SDL_Surface * image = nullptr;

    SDL_Texture * getTexture(SDL_Renderer * renderer);
    void draw(SDL_Renderer * renderer);
private:
    std::string ztd_file;
    IniReader * ini_reader = nullptr;

    SDL_Texture * texture = nullptr;
};

#endif // ANI_FILE_HPP