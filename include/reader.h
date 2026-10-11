#pragma once


#include <fstream>
#include <string>
#include <unordered_map>
namespace BombOS {

enum Format { BOS, PBOS };

class Reader {
private:
  static char delimeter;
  static std::unordered_map<std::string, Format> headers;

private:
  Format fileFormat;
  std::string filePath;
  std::ifstream stream;


  std::string version;

public:
  Reader(const std::string filePath) : filePath(filePath) {}
  ~Reader();

public:
  void Read();

private:
  const std::string GetNext();
  const bool IsValidFormat(Format &foundFormat);
  void GetVersion();
};
} // namespace BombOS
