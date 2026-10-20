#pragma once


#include "MapSize.h"
#include "Types.h"
#include <string>

namespace BombOS::Map {
struct MapData {
  const std::string name;
  const TextureStore textureStore;
  const MapSize mapSize;
  const StringMap floor;
  const StringMap main;
  const StringMap collisions;


  MapData(const std::string name, const TextureStore textures, const MapSize mapSize,
          const StringMap floor, const StringMap main, const StringMap collisions)
      : name(name), textureStore(textures), mapSize(mapSize), floor(floor), main(main),
        collisions(collisions) {}
};
} // namespace BombOS::Map
