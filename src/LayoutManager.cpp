#include "LayoutManager.hpp"

#include <string>
#include <algorithm>

LayoutManager::LayoutManager(ResourceManager * resource_manager) {
  this->resource_manager = resource_manager;
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
              } else if (action.target == -1 && layout->hasId(action.source)) {
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
              if (layout->hasId(action.source)) {
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

void LayoutManager::Draw(SDL_Renderer * renderer, SDL_FRect * window_rect) {
  for (int layer=0; layer < (8 + 1); layer++) {
    for(auto kv : layouts) {
      UiLayout * layout = layouts[kv.first];
      if (layout == nullptr || layout->getLayer() != layer || !layout->getActive()) {
        continue;
      }
      layout->draw(renderer, window_rect);
    }
  }
}

void LayoutManager::Load(std::atomic<float> * progress, std::atomic<bool> * is_done) {
    if (this->loaded) {
    *progress = 100.0f;
    *is_done = true;
    return;
  }

  // Load all the layouts from ui/gamescrn.lyt
  IniReader * ini_reader = this->resource_manager->getIniReader("ui/gamescrn.lyt");
  float progress_per_layout_load = (100.0f - *progress) / (float) ini_reader->getSections().size();
  for(std::string section : ini_reader->getSections()) {
    if (section == "layoutinfo") {
      this->id = ini_reader->getInt(section, "id");
    } else {
      std::string type = ini_reader->get(section, "type");
      if (type == "UILayout") {
        #ifdef DEBUG
          SDL_Log("Loading layout %s at %s", section.c_str(), ini_reader->get(section, "layout").c_str());
        #endif
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

    // Increase progress
    if (*progress + progress_per_layout_load < 100.0f) {
      *progress = *progress + progress_per_layout_load;
    } else {
      *progress = 100.0f;
    }
  }
  this->loaded = true;
  free(ini_reader);
  *is_done = true;
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
