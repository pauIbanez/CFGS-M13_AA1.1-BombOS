#pragma once

namespace BombOS::Map {
struct MapSize {
  short width, height = 0;

  MapSize(short width, short height) : height(height), width(width) {}
};
} // namespace BombOS::Map
