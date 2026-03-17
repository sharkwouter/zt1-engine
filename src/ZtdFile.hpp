#ifndef ZTD_FILE_HPP
#define ZTD_FILE_HPP

#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "IniReader.hpp"

class ZtdFile {
public:
    static std::vector<std::string> getFileList(const std::string &ztd_file);
    static void * getFileContent(const std::string &ztd_file, const std::string &file_name, int * size);
    static SDL_Surface * getImageSurface(const std::string &ztd_file, const std::string &file_name);
    static MIX_Audio * getMusic(const std::string &ztd_file, const std::string &file_name);
    static IniReader * getIniReader(const std::string &ztd_file, const std::string &file_name);
private:
    static SDL_Surface * getImageSurfaceBmp(const std::string &ztd_file, const std::string &file_name);
    static SDL_Surface * getImageSurfaceTga(const std::string &ztd_file, const std::string &file_name);
    static SDL_Surface * getImageSurfaceZt1(const std::string &ztd_file, const std::string &file_name);
};

#endif // ZTD_FILE_HPP