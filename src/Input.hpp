#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL3/SDL.h>

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
  SDL_FPoint position;
} Input;

#endif // INPUT_HPP