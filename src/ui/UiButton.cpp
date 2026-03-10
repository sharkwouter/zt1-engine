#include "UiButton.hpp"

#include "../CompassDirection.hpp"

UiButton::UiButton(IniReader * ini_reader, ResourceManager * resource_manager, std::string name) {
  this->ini_reader = ini_reader;
  this->resource_manager = resource_manager;
  this->name = name;

  this->id = ini_reader->getInt(name, "id");
  this->layer = ini_reader->getInt(name, "layer", 1);
  this->target = ini_reader->getInt(name, "target", 0);
  this->action = (Action) ini_reader->getInt(name, "action", 0);
  if (ini_reader->getInt(name, "anchor", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor"));
  }
  if (ini_reader->getInt(name, "anchor1", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor1"));
  }
  if (ini_reader->getInt(name, "anchor2", 0) != 0) {
    this->anchors.push_back(ini_reader->getInt(name, "anchor2"));
  }

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
  UiAction result = {Action::NONE, 0, 0};
  for (Input input : inputs) {
    if (input.type != InputType::POSITIONED) {
      continue;
    }
    if (input.position.x < this->draw_rect.x || input.position.x > this->draw_rect.x + this->draw_rect.w) {
      if (this->selected) {
        this->selected_updated = true;
      }
      this->selected = false;
      continue;
    }
    if (input.position.y < this->draw_rect.y || input.position.y > this->draw_rect.y + this->draw_rect.h) {
      if (this->selected) {
        this->selected_updated = true;
      }
      this->selected = false;
      continue;
    }

    if (!this->selected) {
      this->selected_updated = true;
    }
    this->selected = true;
    switch (input.event) {
      case InputEvent::LEFT_CLICK:
        result = {this->action, this->target, this->id};
        break;
      default:
        break;
    }
  }
  if (result.source == 0) {
    result = handleInputChildren(inputs);
  }
  return result;
}

void UiButton::draw(SDL_Renderer * renderer, SDL_Rect * layout_rect) {
  if (!this->text_string.empty() && (this->text == nullptr || (this->selected_updated && this->has_select_color))) {
    std::vector<std::string> color_values;
    if (this->selected && this->has_select_color && !ini_reader->getList(name, "selectcolor").empty()) {
      color_values = ini_reader->getList(name, "selectcolor");
      this->current_button_image = CompassDirection::S;
    } else {
      color_values = ini_reader->getList(name, "forecolor");
      this->current_button_image = CompassDirection::N;
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

  this->generateDrawRect(this->ini_reader->getSection(this->name), layout_rect);
  SDL_Rect text_rect = {draw_rect.x, draw_rect.y, 0, 0};
  if (this->animation != nullptr) {
    this->animation->draw(renderer, &draw_rect, CompassDirection::N);

    // Prepare position for text drawing
    if (this->ini_reader->get(this->name, "justify") == "center") {
      text_rect.x += draw_rect.w / 2;
      text_rect.y += draw_rect.h / 2;
    }
  }

  SDL_QueryTexture(this->text, NULL, NULL, &text_rect.w, &text_rect.h);
  if (this->ini_reader->get(this->name, "justify") == "center") {
    text_rect.x -= text_rect.w / 2;
    text_rect.y -= text_rect.h / 2;
  }

  // Make sure draw_rect has a size so mouse selection works
  if (draw_rect.w == 0 || draw_rect.h == 0) {
    draw_rect = text_rect;
  }

  shadow_rect = {text_rect.x - 1, text_rect.y + 1, text_rect.w, text_rect.h};
  SDL_RenderCopy(renderer, this->shadow, NULL, &shadow_rect);

  SDL_RenderCopy(renderer, this->text, NULL, &text_rect);
  this->drawChildren(renderer, &draw_rect);
}
