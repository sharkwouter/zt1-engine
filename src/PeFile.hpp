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
    std::vector<uint32_t> getStringIds();
    std::string getString(uint32_t string_id);
private:
    PeResourceLoader * loader = nullptr;
};

#endif // PE_FILE_HPP