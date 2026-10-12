#pragma once

#include "MapSize.h"
#include "Types.h"
#include <string>

namespace BombOS::Map {
struct MapData {
  const std::string name;
  const TextureMap textures;
  const MapSize mapSize;

  MapData(const std::string name, const TextureMap textures, const MapSize mapSize)
      : name(name), textures(textures), mapSize(mapSize) {}
};
} // namespace BombOS::Map
