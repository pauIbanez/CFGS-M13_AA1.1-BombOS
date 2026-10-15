#pragma once

#include <string>
#include <vector>

namespace BombOS::Map {
struct FlatMap {
  std::vector<std::string> data;

  size_t width;
  FlatMap(size_t width, size_t height) : data(height * width), width(width) {}
  std::string &at(size_t y, size_t x) { return data[y * width + x]; }
};
} // namespace BombOS::Map
