#pragma once


#include "MapSize.h"
#include "Types.h"
#include <string>

namespace BombOS::Map {
struct MapData {
  const std::string name;
  const TextureStore textureStore;
  const MapSize mapSize;
  StringMap floor;
  StringMap main;
  StringMap collisions;
  StringMap objects;


  MapData(const std::string name, const TextureStore textures, const MapSize mapSize,
          const StringMap floor, const StringMap main, const StringMap collisions,
          const StringMap objects)
      : name(name), textureStore(textures), mapSize(mapSize), floor(floor), main(main),
        collisions(collisions), objects(objects) {}
};
} // namespace BombOS::Map
