#pragma once

#include "FlatMap.h"
#include "MapSize.h"
#include "Types.h"
#include <string>

namespace BombOS::Map {
struct MapData {
  const std::string name;
  const TextureStore textureStore;
  const MapSize mapSize;
  const FlatMap floor;
  const FlatMap main;
  const FlatMap collisions;


  MapData(const std::string name, const TextureStore textures, const MapSize mapSize,
          const FlatMap floor, const FlatMap main, const FlatMap collisions)
      : name(name), textureStore(textures), mapSize(mapSize), floor(floor), main(main),
        collisions(collisions) {}
};
} // namespace BombOS::Map
