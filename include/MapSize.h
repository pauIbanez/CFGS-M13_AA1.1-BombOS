#pragma once

namespace BombOS::Map {
struct MapSize {
  short height, width = 0;

  MapSize(short height, short width) : height(height), width(width) {}
};
} // namespace BombOS::Map
