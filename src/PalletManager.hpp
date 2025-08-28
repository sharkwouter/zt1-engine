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

  Pallet * getPallet(char * file_name_c);
  Pallet * getPallet(std::string &file_name);

  void addPalletFileToMap(const std::string &pallet_file, std::string ztd_file);
  void loadPalletMap(std::atomic<float> * progress, float progress_goal);

private:
  std::unordered_map<std::string, std::string> pallet_files_map;
  std::unordered_map<std::string, Pallet> pallet_map;

  void loadPallet(const std::string &file_name);

  bool loaded = false;
};

#endif // PALLET_MANAGER_HPP