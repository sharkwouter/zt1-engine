#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP

#include <unordered_map>
#include <string>

#include <SDL2/SDL_ttf.h>


class FontManager {
public:
  FontManager();
  ~FontManager();

  SDL_Texture * getStringTexture(SDL_Renderer * renderer, const int font, const std::string &string, SDL_Color color);

private:
  std::unordered_map<int, TTF_Font *> fonts;

  void loadFont(const int font);
};

#endif // FONT_MANAGER_HPP