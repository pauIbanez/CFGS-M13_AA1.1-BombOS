#pragma once

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
  const TextureMap ReadTextures();
  const Map::MapSize ReadSize();

private:
  void ReadTexturesBOS(TextureMap &textures);
  void ReadTexturesPBOS(TextureMap &textures);

private:
  const bool TryParseFormat(std::string &formatStr, Format &format);
  const bool TryParseVersion(std::string &versionStr, Version *&version);

  const std::string GetNext(const char &del = delimeter);
  void ExpectHeader(const std::string &header);
  const bool IsLineEnd(const std::string &read) const;
  void ExpectLineEnd(const std::string &after);
  void ExpectEnd();
};
} // namespace BombOS
