#include "IniReader.hpp"

#include <SDL2/SDL.h>

static void print_content(std::unordered_map<std::string, std::unordered_map<std::string, std::string>> content) {
  SDL_Log("Printing loaded content");
  std::string section = "";
  for (auto section_key : content) {
    section = section_key.first;
    SDL_Log("Section %s:", section.c_str());
    for (auto key_value : section_key.second) {
      SDL_Log("- %s: %s", key_value.first.c_str(), key_value.second.c_str());
    }
  }
}

IniReader::IniReader(const std::string &filename) {
  SDL_Log("Opening file %s", filename.c_str());
  FILE * fd = fopen(filename.c_str(), "r");
  if (fd == NULL) {
    SDL_Log("Could not open ini file %s", filename.c_str());
  }
  fseek(fd, 0L, SEEK_END);
  size_t size = ftell(fd) + 1;
  fseek(fd, 0, SEEK_SET);
  void * buffer = malloc(size);
  fread(buffer, sizeof(char), size, fd);
  load(std::string((char *) buffer, size));

  // print_content(content);
}

IniReader::IniReader(void *buffer, size_t size) {
  load(std::string((char *) buffer, size));
  
  // print_content(content);
}

IniReader::~IniReader() {

}

static std::string string_to_lower(const std::string &value) {
  std::string new_string = "";
  for (char character : value) {
    new_string += std::tolower(character);
  }
  return new_string;
}

std::string IniReader::get(const std::string &section, const std::string &key, const std::string &default_value) {
  std::string section_lower = string_to_lower(section);
  std::string key_lower = string_to_lower(key);
  if (content.count(section_lower) != 0 && content[section_lower].count(key_lower) != 0) {
    return content[section_lower][key_lower];
  }
  SDL_Log("Value of %s: %s not found, returning default", section.c_str(), key.c_str());
  return default_value;
}

int IniReader::getInt(const std::string &key, const std::string &value, const int &default_value)
{
  int result = default_value;
  std::string string_value = get(key, value);
  if (!string_value.empty()) {
    result = std::stoi(string_value, NULL, 10);
  }
  return result;
}

void IniReader::load(std::string file_content) {
  size_t character_number = 0;
  bool skip_to_end_of_line = false;
  bool skip_to_value = false;
  std::string current_section = "";
  std::string current_key = "";
  std::string current_value = "";

  typedef enum {
    SECTION,
    KEY,
    VALUE
  } process_mode;
  process_mode current_mode = process_mode::SECTION;

  
  for (char character : file_content) {
    // We don't need to read comments
    if (skip_to_end_of_line && character != '\n') {
      continue;
    }
    // In between the key and the value we can ignore spaces and the equals
    if (skip_to_value && (character == ' ' || character == '=')) {
      continue;
    }
    // We don't want windows newline characters in our map
    if (character == '\r') {
      continue;
    }
    // On newline save the data if we're in value mode, otherwise just reset the character number
    if (character == '\n') {
      if (current_mode == process_mode::VALUE && skip_to_end_of_line == false) {
        if (!current_section.empty() && !current_key.empty() && !current_value.empty()) {
          content[current_section][current_key] = current_value;
        }
        current_key = "";
        current_value = "";
      }
      skip_to_end_of_line = false;
      skip_to_value = false;
      character_number = 0;
      continue;
    }
    // Set the mode at the beginning of the line
    if (character_number == 0) {
      switch (character) {
        case '[':
          current_mode = process_mode::SECTION;
          current_section = "";
          character_number++;
          continue;
          break;
        case ';':
          skip_to_end_of_line = true;
          continue;
          break;
        case ' ':
          //I feel it is risky to assume that a line starting with a space is empty, but we do it anyway 
          skip_to_end_of_line = true;
          continue;
          break;
      default:
        // By default we assume we're dealing with a line with a key and value. This might not always apply, but I've not seen it.
        current_mode = process_mode::KEY;
        current_key = "";
        break;
      }
    }
    switch (current_mode) {
      case process_mode::SECTION:
        if (character == ']') {
          content[current_section] = std::unordered_map<std::string, std::string>();
          continue;
        }
        current_section += std::tolower(character);
        break;
      case process_mode::KEY:
        if (character == ' ' || character == '=') {
          current_mode = process_mode::VALUE;
          skip_to_value = true;
          continue;
        }
        current_key += std::tolower(character);
        break;
      case process_mode::VALUE:
        if (skip_to_value) {
          skip_to_value = false;
        }
        current_value += character;
        break;
    }
    character_number++;
  }
}
