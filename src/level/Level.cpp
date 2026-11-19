#include "Level.h"
#include "Assets.h"
#include "Reader.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <memory>
#include <raylib-cpp/Vector2.hpp>

namespace BombOS {

std::unique_ptr<Map::MapData> Level::loadedMap;
std::unique_ptr<Map::MapData> Level::mapData;
std::vector<std::unique_ptr<Player>> Level::players;
std::vector<std::unique_ptr<Bomb>> Level::bombs;

void Level::Load(const std::string &levelName) {
  Reader reader(levelName);
  loadedMap = reader.Read();
  if (!loadedMap->music.empty()) Assets::LoadMusic(loadedMap->music);

  Assets::LoadTextures(loadedMap->textureStore);

  Start();
}

void Level::Start() {
  if (mapData) *mapData = *loadedMap;
  else mapData = std::make_unique<Map::MapData>(*loadedMap);

  players.clear();
  SpawnPlayers();
  Assets::GetMusic().Seek(0);
  Assets::GetMusic().Play();
}

void Level::Update() {
  for (const std::unique_ptr<Player> &player : players)
    player->Update();

  for (const std::unique_ptr<Bomb> &bomb : bombs)
    bomb->Update();

  std::erase_if(bombs, [](const auto &b) { return b->hasDetonated; });
}

void Level::Unload() {
  players.clear();
  bombs.clear();
  mapData.reset();
  loadedMap.reset();
}

bool Level::IsSolidAt(const raylib::Vector2 pos) {
  if (pos.x < 0 || pos.y < 0) return true;

  const int x = (int)pos.x;
  const int y = (int)pos.y;
  const Map::MapSize &size = mapData->mapSize;
  if (x >= size.width || y >= size.height) return true;

  if (mapData->collisions.Read(x, y) != "0") return true;
  const raylib::Vector2 tilePos{(float)x, (float)y};
  auto it = std::find_if(
      bombs.begin(), bombs.end(),
      [&tilePos](const std::unique_ptr<Bomb> &bomb) { return bomb->position == tilePos; }

  );

  if (it == bombs.end()) return false;

  return it->get()->IsSolid();
}



void Level::SpawnBomb(const raylib::Vector2 &position) {
  raylib::Vector2 pos{std::floor(position.x), std::floor(position.y)};

  auto it = std::find_if(bombs.begin(), bombs.end(),
                         [&pos](const std::unique_ptr<Bomb> &bomb) { return bomb->position == pos; }

  );

  if (it == bombs.end()) bombs.push_back(std::make_unique<Bomb>(pos));
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

bool Level::DestroyBlock(const raylib::Vector2 &pos) {
  if (pos.x < 0 || pos.y < 0) return true;

  const int x = (int)pos.x;
  const int y = (int)pos.y;
  const Map::MapSize &size = mapData->mapSize;
  if (x >= size.width || y >= size.height) return true;

  std::string &col = mapData->collisions.At(x, y);
  if (col == "0") return false;

  if (col != "D") return true;
  col = "0";
  mapData->main.At(x, y) = "0";
  return true;
}
} // namespace BombOS
