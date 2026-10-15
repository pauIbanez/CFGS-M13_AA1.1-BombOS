#pragma once

#include "MapSize.h"
#include "Reader.h"



namespace BombOS {


class BOSReader : public Reader {

public:
  BOSReader(std::ifstream &&stream) : Reader(std::move(stream)) {}

public:
  const std::unique_ptr<Map::MapData> Read() override;

private:
  const Map::TextureStore ReadTextures();
  const Map::MapSize ReadSize();

  const Map::StringMap ReadFloor(const Map::TextureStore &store, const Map::MapSize &size);
  const Map::StringMap ReadMap(const Map::TextureStore &store, const Map::MapSize &size);
  const Map::StringMap ReadCollisions(const Map::MapSize &size);


private:
  const bool EnsureRegistered(const Map::TextureStore &store,
                              const std::string &textureIdent) const;
};
} // namespace BombOS
