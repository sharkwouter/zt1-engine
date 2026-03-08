#include "LayoutManager.hpp"

#include <string>
#include <algorithm>

LayoutManager::LayoutManager(ResourceManager * resource_manager) {
  IniReader * ini_reader = resource_manager->getIniReader("ui/gamescrn.lyt");
  generateLayouts(ini_reader, resource_manager);
  free(ini_reader);
}

LayoutManager::~LayoutManager() {
  for(auto kv : layouts) {
    free(kv.second);
    layouts[kv.first] = nullptr;
  }
}

bool LayoutManager::HandleInputs(std::vector<Input> &inputs) {
  for (int layer=8; layer > (0 - 1); layer--) {
    for(auto kv : layouts) {
      UiLayout * layout = layouts[kv.first];
      if (layout == nullptr || layout->getLayer() != layer || !layout->getActive()) {
        continue;
      }
      UiAction action = layout->handleInputs(inputs);
      if (action.source != 0) {
        SDL_Log("Source of input is %i on layer %i", action.source, layer);
        switch (action.action) {
          case Action::SHOW_TARGET_LAYOUT:
            for(auto kv : layouts) {
              UiLayout * layout = layouts[kv.first];
              if (layout->getId() == action.target) {
                layout->setActive(true);
              }
            }
            break;
          case Action::HIDE_TARGET_LAYOUT:
            for(auto kv : layouts) {
              UiLayout * layout = layouts[kv.first];
              if (layout->getId() == action.target) {
                layout->setActive(false);
              } else if (action.target == -1 && layout->getId() == action.source) {
                layout->setActive(false);
              }
            }
            break;
          case Action::TOGGLE_TARGET_LAYOUT:
            for(auto kv : layouts) {
              UiLayout * layout = layouts[kv.first];
              if (layout->getId() == action.target) {
                layout->setActive(!layout->getActive());
              }
            }
            break;
          case Action::SWITCH_TO_TARGET_LAYOUT:
            for(auto kv : layouts) {
              UiLayout * layout = layouts[kv.first];
              if (layout->getId() == action.target) {
                layout->setActive(true);
              }
              if (layout->getId() == action.source) {
                layout->setActive(false);
              }
            }
            break;
          case Action::NONE:
            {
              bool running = handleTargetlessAction(action);
              if (!running)
                return false;
            }
            break;
          default:
            break;
        }
        return true;
      }
    }
  }
  return true;
}

void LayoutManager::Draw(SDL_Renderer *renderer, SDL_Rect *layout_rect) {
  for (int layer=0; layer < (8 + 1); layer++) {
    for(auto kv : layouts) {
      UiLayout * layout = layouts[kv.first];
      if (layout == nullptr || layout->getLayer() != layer || !layout->getActive()) {
        continue;
      }
      layout->draw(renderer, layout_rect);
    }
  }
}

void LayoutManager::generateLayouts(IniReader *ini_reader, ResourceManager * resource_manager) {
  for(std::string section : ini_reader->getSections()) {
    if (section == "layoutinfo") {
      this->id = ini_reader->getInt(section, "id");
      continue;
    }

    std::string type = ini_reader->get(section, "type");
    if (type == "UILayout") {
      SDL_Log("Loading layout %s", section.c_str());
      std::string layout = ini_reader->get(section, "layout");
      if (layout != "ui/infocomC.lyt") {
        // infocomC.lyt is the only lyt file that has a capital letter in the name
        std::transform(layout.begin(), layout.end(), layout.begin(), ::tolower);
      }
      this->layouts[section] = new UiLayout(ini_reader, resource_manager, section);
    } else {
      // TODO: Implement support for ZTMapView, ZTMessageQueue and UIText here
      SDL_Log("Cannot load elements of type %s in layout manager yet, not implemented", type.c_str());
    }
  }
}

bool LayoutManager::handleTargetlessAction(UiAction action) {
  switch (action.source) {
    case (int) ActionSource::MAIN_MENU_EXIT:
      return false;
      break;
    default:
      break;
  }
  return true;
}
