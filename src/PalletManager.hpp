#ifndef PALLET_MANAGER_HPP
#define PALLET_MANAGER_HPP

#include <unordered_map>
#include <string>
#include <atomic>

#include "Pallet.hpp"


class PalletManager {
public:
  PalletManager();
  ~PalletManager();

  Pallet * getPallet(std::string &file_name);

  void addPalletFileToMap(std::pair<std::string, std::string> * pallet_map_item);
  void loadPalletMap(std::atomic<float> * progress, float progress_goal);

private:
  std::unordered_map<std::string, std::string> pallet_files_map;
  std::unordered_map<std::string, Pallet> pallet_map;

  void loadPallet(std::string file_name);

  bool loaded = false;
};

#endif // PALLET_MANAGER_HPP