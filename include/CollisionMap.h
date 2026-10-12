#pragma once


#include <vector>

namespace BombOS::Map {
struct CollisionMap {
  std::vector<bool> data;

  size_t width;
  CollisionMap(size_t height, size_t width) : data(height * width), width(width) {}
  bool at(size_t y, size_t x) const { return data[y * width + x]; }
  void set(size_t y, size_t x, bool value) { data[y * width + x] = value; }
};
} // namespace BombOS::Map
