#include "Level.h"
#include "Assets.h"
#include "Reader.h"
#include <charconv>

namespace BombOS {

std::unique_ptr<Map::MapData> Level::loadedMap;
std::unique_ptr<Map::MapData> Level::mapData;
std::vector<std::unique_ptr<Player>> Level::players;

void Level::Load(const std::string &levelName) {
  Reader reader(levelName);
  loadedMap = reader.Read();

  Assets::LoadTextures(loadedMap->textureStore);

  Start();
}

void Level::Start() {
  if (mapData) *mapData = *loadedMap;
  else mapData = std::make_unique<Map::MapData>(*loadedMap);

  // Reset positions
  players.clear();
  SpawnPlayers();
}

void Level::Update() {
  for (const std::unique_ptr<Player> &player : players)
    player->Update();
}

void Level::Unload() {
  players.clear();
  mapData.reset();
  loadedMap.reset();
}

void Level::SpawnPlayers() {

  for (auto [x, y, object] : mapData->objects.GetWithIndex()) {
    short player;
    const char *begin = object.data();
    const char *end = begin + object.size();
    auto [ptr, ec] = std::from_chars(begin, end, player);
    if (ec != std::errc() || ptr != end || player == 0) continue;

    players.push_back(std::make_unique<Player>(player, raylib::Vector2{(float)x, (float)y}));
  }
}
} // namespace BombOS
