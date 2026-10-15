#pragma once

#include <string>
#include <vector>


namespace BombOS::Map {
template <typename T = std::string> struct FlatMap {
  std::vector<T> data;

  size_t width;
  FlatMap(size_t width, size_t height) : data(height * width), width(width) {}
  T &at(size_t x, size_t y) { return data[y * width + x]; }
};
} // namespace BombOS::Map
