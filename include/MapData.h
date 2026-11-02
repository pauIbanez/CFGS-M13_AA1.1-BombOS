#pragma once


#include "MapSize.h"
#include "Types.h"
#include <string>

namespace BombOS::Map {
struct MapData {
  // Not const so a whole MapData can be reassigned (resetting a level from its loaded base)
  std::string name;
  TextureStore textureStore;
  MapSize mapSize;
  StringMap floor;
  StringMap main;
  StringMap collisions;
  StringMap objects;


  MapData(const std::string name, const TextureStore textures, const MapSize mapSize,
          const StringMap floor, const StringMap main, const StringMap collisions,
          const StringMap objects)
      : name(name), textureStore(textures), mapSize(mapSize), floor(floor), main(main),
        collisions(collisions), objects(objects) {}


  MapData(const MapData &base) = default;
  MapData &operator=(const MapData &base) = default;
};
} // namespace BombOS::Map
