#pragma once

#include <cstddef>
#include <string>
#include <vector>


namespace BombOS::Map {
template <typename T = std::string> struct FlatMap {
private:
  std::vector<T> data;
  size_t width;

public:
  FlatMap(size_t width, size_t height) : data(height * width), width(width) {}
  T &At(size_t x, size_t y) { return data[y * width + x]; }
  const T &Read(size_t x, size_t y) const { return data[y * width + x]; }

  std::vector<T> Get() { return data; };
  size_t Size() { return data.size(); }
};
} // namespace BombOS::Map
