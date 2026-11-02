#pragma once

#include <cstddef>
#include <string>
#include <utility>
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

  const std::vector<T> &Get() { return data; };

  template <typename U> struct Cell {
    size_t x;
    size_t y;
    U &value;
  };

  template <typename Map, typename U> struct IndexedView {
    Map *map;

    struct Iterator {
      Map *map;
      size_t i;

      Cell<U> operator*() const { return {i % map->width, i / map->width, map->data[i]}; }
      Iterator &operator++() {
        ++i;
        return *this;
      }
      bool operator!=(const Iterator &other) const { return i != other.i; }
    };

    Iterator begin() const { return {map, 0}; }
    Iterator end() const { return {map, map->data.size()}; }
  };

  IndexedView<FlatMap, T> GetWithIndex() { return {this}; }
  IndexedView<const FlatMap, const T> GetWithIndex() const { return {this}; }

  size_t Size() { return data.size(); }
};
} // namespace BombOS::Map
