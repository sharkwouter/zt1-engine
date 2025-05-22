#include "InputManager.hpp"

InputManager::InputManager() {

}

InputManager::~InputManager() {

}

std::vector<Input> InputManager::getInputs() {
  std::vector<Input> inputs;
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    Input input = {
      .type = InputType::NONE,
      .event = InputEvent::NONE,
      .position = {0, 0}
    };

    switch (event.type) {
      case SDL_QUIT:
        input.type = InputType::BUTTON;
        input.event = InputEvent::QUIT;
        break;
      case SDL_KEYDOWN:
        input.type = InputType::BUTTON;
        input.event = InputEvent::NONE;
        break;
      case SDL_MOUSEBUTTONDOWN:
        input.type = InputType::POSITIONED;
        SDL_GetMouseState(&input.position.x, &input.position.y);
        input.event = getEventFromMouseButton(event.button.button);
        break;
      case SDL_MOUSEMOTION:
        input.type = InputType::POSITIONED;
        input.event = InputEvent::CURSOR_MOVE;
        SDL_GetMouseState(&input.position.x, &input.position.y);
        break;
    }
    if (input.type != InputType::NONE && input.event != InputEvent::NONE) {
        inputs.push_back(input);
    }
  }

  return inputs;
}

InputEvent InputManager::getEventFromMouseButton(Uint8 button) {
    InputEvent event;

    switch (button) {
        case SDL_BUTTON_LEFT:
            event = InputEvent::LEFT_CLICK;
            break;
        case SDL_BUTTON_RIGHT:
            event = InputEvent::RIGHT_CLICK;
            break;
        case SDL_BUTTON_MIDDLE:
            event = InputEvent::NONE;
            break;
        default:
            event = InputEvent::NONE;
            break;
    }

    return event;
}