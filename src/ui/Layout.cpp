#include "Layout.hpp"

Layout::Layout(IniReader * ini_reader) {
  this->name = "LayoutInfo";
  this->dest_rect = this->getRect(*(ini_reader->getSection(name)));
  this->id = ini_reader->getInt(name, "id", 0);
  this->layer = ini_reader->getInt(name, "id", 0);
}

Layout::~Layout() {

}

void Layout::draw(SDL_Renderer *renderer) {

}
