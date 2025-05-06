#ifndef PE_FILE_HPP
#define PE_FILE_HPP

#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <pe_resource_loader.h>

class PeFile {
public:
    PeFile(const std::string &pe_file);
    ~PeFile();

    SDL_Surface * getLoadScreenSurface();
private:
    PeResourceLoader * loader = nullptr;
};

#endif // PE_FILE_HPP