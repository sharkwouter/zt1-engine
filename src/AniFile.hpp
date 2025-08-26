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
#include "AnimationData.hpp"
#include "Animation.hpp"

class AniFile {
public:
    static Animation * getAnimation(PalletManager * pallet_manager, const std::string &ztd_file, const std::string &file_name);
private:
    static std::string getAnimationDirectory(IniReader * ini_reader);
    static AnimationData * loadAnimationData(PalletManager * pallet_manager, const std::string &ztd_file, const std::string &directory);
};

#endif // ANI_FILE_HPP