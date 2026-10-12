#pragma once

#include <string>
#include <vector>

namespace BombOS::Map {
struct TextureMap {
  std::vector<std::string> data;

  size_t width;
  TextureMap(size_t height, size_t width) : data(height * width), width(width) {}
  std::string &at(size_t y, size_t x) { return data[y * width + x]; }
};
} // namespace BombOS::Map
