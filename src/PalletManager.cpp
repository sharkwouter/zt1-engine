#include "PalletManager.hpp"

#include "ZtdFile.hpp"

PalletManager::PalletManager() {
}

PalletManager::~PalletManager() {

}

Pallet * PalletManager::getPallet(char * file_name_c) {
  std::string file_name = std::string(file_name_c);
  return this->getPallet(file_name);
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

void PalletManager::addPalletFileToMap(const std::string &pallet_file, std::string ztd_file) {
  this->pallet_files_map[pallet_file] = ztd_file;
}

void PalletManager::loadPalletMap(std::atomic<float> * progress, std::atomic<bool> * is_done) {
  if (this->loaded) {
    *progress = 100.0f;
    *is_done = true;
    return;
  }

  float progress_per_pallet_load = (100.0f - *progress) / (float) this->pallet_files_map.size();
  for (auto pallet_file_map_item : this->pallet_files_map) {
    std::string file_name = pallet_file_map_item.first; 

    // Increase progress bar position
    if (*progress + progress_per_pallet_load < 100.0f) {
      *progress = *progress + progress_per_pallet_load;
    } else {
      *progress = 100.0f;
    }
  
    // Skip ztatb in initial load, there are just too many of them
    if (file_name.starts_with("ztatb/")) {
      continue;
    }

    this->loadPallet(file_name);
  }

  this->loaded = true;
  *is_done = true;
}

void PalletManager::loadPallet(const std::string &file_name) {
    Pallet pallet;
    int pallet_file_size = 0;
    void * pallet_file_content = ZtdFile::getFileContent(this->pallet_files_map[file_name], file_name, &pallet_file_size);
    if (pallet_file_content == nullptr) {
      SDL_Log("Could not load pallet %s from %s, returning", file_name.c_str(), this->pallet_files_map[file_name].c_str());
      return;
    }
    SDL_IOStream * pallet_rw = SDL_IOFromMem(pallet_file_content, pallet_file_size);

    SDL_ReadU32LE(pallet_rw, &pallet.color_count);
    SDL_ReadIO(pallet_rw, pallet.colors, (size_t) pallet.color_count * sizeof(uint32_t));
    SDL_CloseIO(pallet_rw);

    this->pallet_map[file_name] = pallet;
}
