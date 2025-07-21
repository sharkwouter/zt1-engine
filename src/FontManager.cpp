#include "FontManager.hpp"

#include <SDL2/SDL.h>

#include "Utils.hpp"

FontManager::FontManager() {

}

FontManager::~FontManager() {
  for (auto font_entry : this->fonts) {
    TTF_CloseFont(font_entry.second);
  }
}

SDL_Texture * FontManager::getStringTexture(SDL_Renderer * renderer, const int font, const std::string &string, SDL_Color color) {
  this->loadFont(font);

  SDL_Surface * surface = TTF_RenderUTF8_Blended(this->fonts[font], string.c_str(), color);
  if (surface == NULL) {
      SDL_Log("Couldn't create surface for text %s: %s", string.c_str(), TTF_GetError());
      return NULL;
  }

  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  return texture;
}

void FontManager::loadFont(const int font) {
  if (this->fonts.contains(font)) {
    return;
  }

  int font_size = 0;
  std::string font_file;
  switch (font) {
    case 7100:  // Largest options in main menu
      font_file = "Aileron-Black.otf";
      font_size = 18;
      break;
    case 7108:  // Smaller options in main menu
      font_file = "Aileron-Black.otf";
      font_size = 16;
      break;
    default:
      SDL_Log("Could not load unknown font %i, using default", font);
      font_file = "Aileron-Regular.otf";
      font_size = 14;
      break;
  }

  this->fonts[font] = TTF_OpenFont(std::string(Utils::getExecutableDirectory() +"font/" + font_file).c_str(), font_size);
}
