#pragma once


#include "Version.h"
#include <fstream>
#include <string>
#include <unordered_map>


namespace BombOS {

enum Format { BOS, PBOS };

class Reader {

private:
  static const char delimeter;
  static const std::unordered_map<std::string, Format> headers;
  static const Version PBOSlatest;
  static const Version BOSlatest;

private:
  std::string filePath;
  std::ifstream stream;

  Format format;
  Version *version = nullptr;

public:
  Reader(const std::string filePath) : filePath(filePath) {}
  ~Reader();

public:
  void Read();

private:
  void ReadFormat();
  void ReadVersion();

private:
  const bool TryGetFormat(Format &format, std::string &header);
  const bool TryGetVersion(Version *&version, std::string &versionStr);


  const std::string GetNext(const char &del = delimeter);
};
} // namespace BombOS
