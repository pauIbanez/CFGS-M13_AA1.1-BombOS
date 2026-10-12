#pragma once

#include "Reader.h"



namespace BombOS {


class PBOSReader : public Reader {

public:
  PBOSReader(std::ifstream &&stream) : Reader(std::move(stream)) {}

public:
  const std::unique_ptr<Map::MapData> Read() override;

private:
  const Map::TextureStore ReadTextures();

private:
  void ReadTexturesPBOS(Map::TextureStore &textures);
};
} // namespace BombOS
