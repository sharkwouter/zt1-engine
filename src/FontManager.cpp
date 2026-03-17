#include "FontManager.hpp"

#include <SDL3/SDL.h>

#include "Utils.hpp"

FontManager::FontManager() {

}

FontManager::~FontManager() {
  for (auto font_entry : this->fonts) {
    if (font_entry.second != nullptr) {
      TTF_CloseFont(font_entry.second);
    }
  }
  TTF_Quit();
}

SDL_Texture * FontManager::getStringTexture(SDL_Renderer * renderer, const int font, const std::string &string, SDL_Color color) {
  this->loadFont(font);

  SDL_Surface * surface = TTF_RenderText_Blended(this->fonts[font], string.c_str(), string.length(), color);
  if (surface == NULL) {
      SDL_Log("Couldn't create surface for text %s: %s", string.c_str(), SDL_GetError());
      return NULL;
  }

  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);

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
      font_file = "Aileron-Bold.otf";
      font_size = 16;
      break;
    case 4736:  // Version information in main menu
      font_file = "Aileron-Black.otf";
      font_size = 12;
      break;
    case 14004:  // Map description text in freeform menu
    case 11520:  // Labels in freeform
      font_file = "Aileron-Black.otf";
      font_size = 12;
      break;
    case 11522: // Back buttons in map and scenario select
      font_file = "Aileron-Black.otf";
      font_size = 12;
      break;
    case 14000: // Buttons in map and scenario select
      font_file = "Aileron-Black.otf";
      font_size = 12;
      break;
    default:
      SDL_Log("Could not load unknown font %i, using default", font);
      font_file = "Aileron-Regular.otf";
      font_size = 14;
      break;
  }

  this->fonts[font] = TTF_OpenFont(Utils::fixPath("font/" + font_file).c_str(), font_size);
}
