#include "PalletManager.hpp"

#include "ZtdFile.hpp"

PalletManager::PalletManager() {
}

PalletManager::~PalletManager() {
}

Pallet * PalletManager::getPallet(std::string &file_name) {
  if (this->pallet_map.contains(file_name)) {
    return &this->pallet_map[file_name];
  }

  if (this->pallet_files_map.contains(file_name)) {
    this->loadPallet(file_name);
    return &this->pallet_map[file_name];
  }

  SDL_Log("Could not find pallet file %s, returning nullptr", file_name.c_str());
  return nullptr;
}

void PalletManager::addPalletFileToMap(std::pair<std::string, std::string> * pallet_map_item) {
  this->pallet_files_map[pallet_map_item->first] = pallet_map_item->second;
}

void PalletManager::loadPalletMap(std::atomic<float> * progress, float progress_goal) {
  if (this->loaded) {
    return;
  }

  float progress_per_pallet_load = (progress_goal - *progress) / (float) this->pallet_files_map.size();
  for (std::pair<std::string, std::string> pallet_file_map_item : this->pallet_files_map) {
    std::string file_name = pallet_file_map_item.first; 

    // Increase progress bar position
    if (*progress + progress_per_pallet_load < progress_goal) {
      *progress = *progress + progress_per_pallet_load;
    } else {
      *progress = progress_goal;
    }
  
    // Skip ztatb in initial load, there are just too many of them
    if (file_name.starts_with("ztatb/")) {
      continue;
    }

    this->loadPallet(file_name);

  }

  this->loaded = true;
}

void PalletManager::loadPallet(std::string file_name) {
    Pallet pallet;
    int pallet_file_size = 0;
    void * pallet_file_content = ZtdFile::getFileContent(this->pallet_files_map[file_name], file_name, &pallet_file_size);
    SDL_RWops * pallet_rw = SDL_RWFromMem(pallet_file_content, pallet_file_size);

    pallet.color_count =  SDL_ReadLE32(pallet_rw);
    SDL_RWread(pallet_rw, pallet.colors, sizeof(uint32_t), (size_t) pallet.color_count);
    SDL_RWclose(pallet_rw);

    this->pallet_map[file_name] = pallet;
}
