#pragma once

#include "FlatMap.h"
#include "MapData.h"
#include "MapSize.h"
#include "Types.h"
#include "Version.h"
#include <fstream>
#include <string>
#include <unordered_map>



namespace BombOS {


class Reader {

private:
  static const char delimeter;
  static const std::unordered_map<std::string, Format> formatHeaders;
  static const Version PBOSlatest;
  static const Version BOSlatest;

private:
  std::string filePath;
  std::ifstream stream;

  Format format;
  Version *version = nullptr;

  std::unique_ptr<Map::MapData> mapData;

public:
  Reader(const std::string filePath) : filePath(filePath) {}
  ~Reader();

public:
  const std::unique_ptr<Map::MapData> Read();

private:
  void ReadFormat();
  void ReadVersion();

  const std::string ReadTitle();
  const Map::TextureStore ReadTextures();
  const Map::MapSize ReadSize();

  const Map::FlatMap ReadFloor(const Map::TextureStore &store, const Map::MapSize &size);
  const Map::FlatMap ReadMap(const Map::TextureStore &store, const Map::MapSize &size);
  const Map::FlatMap ReadCollisions(const Map::MapSize &size);

private:
  void ReadTexturesBOS(Map::TextureStore &textures);
  void ReadTexturesPBOS(Map::TextureStore &textures);

private:
  const bool TryParseFormat(std::string &formatStr, Format &format);
  const bool TryParseVersion(std::string &versionStr, Version *&version);

private:
  const bool EnsureRegistered(const Map::TextureStore &store,
                              const std::string &textureIdent) const;


private:
  const std::string GetNext(const char &del = delimeter);
  void ExpectHeader(const std::string &header);
  const bool IsLineEnd(const std::string &read) const;
  void ExpectLineEnd(const std::string &after);
  void ExpectEnd();
};
} // namespace BombOS
