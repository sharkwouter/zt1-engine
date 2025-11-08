#include "UiButton.hpp"

#include "../CompassDirection.hpp"

UiButton::UiButton(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);
  this->anchor = ini_reader->getInt(name, "anchor", 0);

  this->has_select_color = !ini_reader->get(name, "selectcolor", "").empty();

  this->font = ini_reader->getInt(name, "font");

  uint32_t string_id = (uint32_t) ini_reader->getUnsignedInt(name, "textid");
  this->text_string = this->resource_manager->getString(string_id);

  std::string animation_path = ini_reader->get(name, "animation");
  if (!animation_path.empty()) {
    this->animation = resource_manager->getAnimation(animation_path);
  }
}

UiButton::~UiButton() {
  SDL_DestroyTexture(text);
  for (UiElement * child : this->children) {
    free(child);
  }
}

UiAction UiButton::handleInputs(std::vector<Input> &inputs) {
  UiAction action = UiAction::NONE;
  for (Input input : inputs) {
    if (input.type != InputType::POSITIONED) {
      continue;
    }
    if (input.position.x < this->dest_rect.x || input.position.x > this->dest_rect.x + this->dest_rect.w) {
      this->selected_updated = true;
      this->selected = false;
      continue;
    }
    if (input.position.y < this->dest_rect.y || input.position.y > this->dest_rect.y + this->dest_rect.h) {
      this->selected_updated = true;
      this->selected = false;
      continue;
    }
    this->selected = true;
    this->selected_updated = true;
    switch (input.event) {
      case InputEvent::LEFT_CLICK:
        if(this->ini_reader->getInt(this->name, "action", 0) == 1) {
          int target = this->ini_reader->getInt(this->name, "target", 0);
          if (target != 0) {
            action = (UiAction) target;
          }
        } else if(this->ini_reader->getInt(this->name, "action", 0) == 2){
          action = UiAction::CREDITS_EXIT;
        } else {
          action = this->getActionBasedOnName();
        }
        break;
      default:
        break;
    }
    
  }
  handleInputChildren(inputs);
  return action;
}

void UiButton::draw(SDL_Renderer * renderer, SDL_Rect * layout_rect) {
  if (!this->text_string.empty() && (this->text == nullptr || (this->selected_updated && this->has_select_color))) {
    std::vector<std::string> color_values;
    if (this->selected && this->has_select_color && !ini_reader->getList(name, "selectcolor").empty()) {
      color_values = ini_reader->getList(name, "selectcolor");
    } else {
      color_values = ini_reader->getList(name, "forecolor");
    }
    SDL_Color color = {0, 0, 0, 255};
    if (color_values.size() == 3) {
      color = {
        (uint8_t) std::stoi(color_values[0]),
        (uint8_t) std::stoi(color_values[1]),
        (uint8_t) std::stoi(color_values[2]),
        255,
      };
    }
    this->text = this->resource_manager->getStringTexture(renderer, this->font, this->text_string, color);
    this->shadow = this->resource_manager->getStringTexture(renderer, this->font, this->text_string, {0, 0, 0, 255});
  }

  dest_rect = this->getRect(this->ini_reader->getSection(this->name), layout_rect);
  SDL_Rect text_rect = {dest_rect.x, dest_rect.y, 0, 0};
  if (this->animation != nullptr) {
    this->animation->draw(renderer, &dest_rect, CompassDirection::N);

    // Prepare position for text drawing
    if (this->ini_reader->get(this->name, "justify") == "center") {
      text_rect.x += dest_rect.w / 2;
      text_rect.y += dest_rect.h / 2;
    }
  }

  SDL_QueryTexture(this->text, NULL, NULL, &text_rect.w, &text_rect.h);
  if (this->ini_reader->get(this->name, "justify") == "center") {
    text_rect.x -= text_rect.w / 2;
    text_rect.y -= text_rect.h / 2;
  }

  // Make sure dest_rect has a size so mouse selection works
  if (dest_rect.w == 0 || dest_rect.h == 0) {
    dest_rect = text_rect;
  }

  shadow_rect = {text_rect.x - 1, text_rect.y + 1, text_rect.w, text_rect.h};
  SDL_RenderCopy(renderer, this->shadow, NULL, &shadow_rect);

  SDL_RenderCopy(renderer, this->text, NULL, &text_rect);
  this->drawChildren(renderer, &dest_rect);
}

UiAction UiButton::getActionBasedOnName() {
  UiAction action = UiAction::NONE;
  if (this->name == "exit") {
    action = UiAction::STARTUP_EXIT;
  } else if (this->name == "back to main menu") {
    action = UiAction::SCENARIO_BACK_TO_MAIN_MENU;
  } else {
    action = UiAction::NONE;
  }
  return action;
}
