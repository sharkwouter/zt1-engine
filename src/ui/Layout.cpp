#include "Layout.hpp"

Layout::Layout(int x, int y, int dx, int dy, int id, int layer) {
  dest_rect = (SDL_Rect *) malloc(sizeof(SDL_Rect));
  *dest_rect = SDL_Rect{x, y, dx, dy};
}

Layout::Layout(int id, int layer) {

}

Layout::~Layout() {
  if (dest_rect != NULL) {
    free(dest_rect);
  }
}

void Layout::draw(SDL_Renderer *renderer) {

}

void Layout::addUiElement(UiElement * element) {
  
}
