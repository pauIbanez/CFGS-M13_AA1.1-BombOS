#pragma once

#include "MapSize.h"
#include "Reader.h"
#include "Version.h"



namespace BombOS {


class BOSReader : public Reader {

public:
  BOSReader(Version version, std::ifstream &&stream) : Reader(std::move(stream)) {
    this->version = new Version(version);
  }

public:
  std::unique_ptr<Map::MapData> Read() override;

private:
  const std::string ReadMusic();
  const Map::TextureStore ReadTextures();
  const Map::MapSize ReadSize();

  const Map::StringMap ReadFloor(const Map::TextureStore &store, const Map::MapSize &size);
  const Map::StringMap ReadMap(const Map::TextureStore &store, const Map::MapSize &size);
  const Map::StringMap ReadCollisions(const Map::MapSize &size);
  const Map::StringMap ReadObjects(const Map::MapSize &size);


private:
  const bool EnsureRegistered(const Map::TextureStore &store,
                              const std::string &textureIdent) const;
};
} // namespace BombOS
