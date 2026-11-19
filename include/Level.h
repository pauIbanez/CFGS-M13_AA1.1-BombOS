#pragma once

#include "Bomb.h"
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
  static std::vector<std::unique_ptr<Bomb>> bombs;

public:
  static inline Map::MapData &GetMapData() { return *mapData; }
  static inline const std::vector<std::unique_ptr<Player>> &GetPlayers() { return players; }
  static inline const std::vector<std::unique_ptr<Bomb>> &GetBombs() { return bombs; }

  static void Load(const std::string &levelName);
  static void Start();
  static void Update();
  static void Unload();
  static bool IsSolidAt(const raylib::Vector2 pos);
  static bool DestroyBlock(const raylib::Vector2 &position);
  static void DetonateBomb(const Bomb *bomb);
  static void SpawnBomb(const raylib::Vector2 &position);

private:
  static void SpawnPlayers();
};
} // namespace BombOS
