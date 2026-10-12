#pragma once


#include "Version.h"
#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>


namespace BombOS {

enum Format { BOS, PBOS };

class Reader {

private:
  static const char delimeter;
  static const std::unordered_map<std::string, Format> formatHeaders;
  static const Version PBOSlatest;
  static const Version BOSlatest;
  static const std::string headers[];

private:
  std::string filePath;
  std::ifstream stream;

  Format format;
  Version *version = nullptr;
  std::string title = "";
  std::map<std::string, std::string> textures;

public:
  Reader(const std::string filePath) : filePath(filePath) {}
  ~Reader();

public:
  void Read();

private:
  void ReadFormat();
  void ReadVersion();
  void ReadTitle();
  void ReadTextures();

private:
  const bool TryGetFormat(Format &format, std::string &header);
  const bool TryGetVersion(Version *&version, std::string &versionStr);
  const bool TryGetTextures(std::vector<std::string> &textures);

  const std::string GetNext(const char &del = delimeter);
  const bool IsLineEnd(const std::string &read) const;
  void ExpectLineEnd(const std::string &after);
};
} // namespace BombOS
