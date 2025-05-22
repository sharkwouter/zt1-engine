#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL2/SDL.h>

enum class InputType {
  NONE,
  POSITIONED,
  BUTTON,
};

enum class InputEvent {
  NONE,
  LEFT_CLICK,
  RIGHT_CLICK,
  CURSOR_MOVE,
  QUIT
};

typedef struct {
  InputType type;
  InputEvent event;
  SDL_Point position;
} Input;

#endif // INPUT_HPP