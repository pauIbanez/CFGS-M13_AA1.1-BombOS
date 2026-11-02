#pragma once

#include "MapData.h"
#include "Player.h"
#include <memory>
#include <raylib-cpp.hpp>
#include <raylib-cpp/Vector2.hpp>
#include <string>
#include <vector>

namespace BombOS {
class Level {
private:
  static std::unique_ptr<Map::MapData> loadedMap;
  static std::unique_ptr<Map::MapData> mapData;
  static std::vector<std::unique_ptr<Player>> players;

public:
  static inline Map::MapData &GetMapData() { return *mapData; }
  static inline const std::vector<std::unique_ptr<Player>> &GetPlayers() { return players; }

  static void Load(const std::string &levelName);
  static void Start();
  static void Update();
  static void Unload();
  static bool IsSolidAt(const raylib::Vector2 pos);

private:
  static void SpawnPlayers();
};
} // namespace BombOS
