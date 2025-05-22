#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <vector>

#include "Input.hpp"

class InputManager {
public:
  InputManager();
  ~InputManager();

  std::vector<Input> getInputs();

private:
  InputEvent getEventFromMouseButton(Uint8 button);
};

#endif // INPUT_MANAGER_HPP