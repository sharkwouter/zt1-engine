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

  print_content(content);
}

IniReader::IniReader(void *buffer, size_t size) {
  load(std::string((char *) buffer, size));
  
  print_content(content);
}

IniReader::~IniReader() {

}

std::string IniReader::get(const std::string &key, const std::string &value, const std::string &default_value) {
  if (content.count(key) != 0 && content[key].count(value) != 0) {
    return content[key][value];
  }
  return default_value;
}

void IniReader::load(std::string file_content) {
  SDL_Log("Read file content:\n%s", file_content.c_str());
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
    if (skip_to_end_of_line && character != '\n') {
      continue;
    }
    if (skip_to_value && (character == ' ' || character == '=')) {
      continue;
    }
    if (character == '\r') {
      continue;
    }
    if (character == '\n') {
      if (current_mode == process_mode::VALUE && skip_to_end_of_line == false) {
        content[current_section][current_key] = current_value;
        current_key = "";
        current_value = "";
      }
      skip_to_end_of_line = false;
      character_number = 0;
      continue;
    }
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
          skip_to_end_of_line = true;
          continue;
          break;
      default:
        current_mode = process_mode::KEY;
        current_key = "";
        break;
      }
    }
    switch (current_mode) {
      case process_mode::SECTION:
        if (character == ']') {
          content[current_section] = std::unordered_map<std::string, std::string>();
          skip_to_end_of_line = true;
          continue;
        }
        current_section += character;
        break;
      case process_mode::KEY:
        if (character == ' ' || character == '=') {
          current_mode = process_mode::VALUE;
          skip_to_value = true;
          continue;
        }
        current_key += character;
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
