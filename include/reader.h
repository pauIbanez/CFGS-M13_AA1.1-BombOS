#pragma once


#include <fstream>
#include <string>
namespace BombOS {
class Reader {
private:
  static char delimeter;
  static std::string header;

private:
  std::string filePath;
  std::ifstream stream;
  std::string version;

public:
  Reader(std::string filePath) : filePath(filePath) {}
  ~Reader();

public:
  void Read();

private:
  std::string GetNext();
  bool IsValidFormat();
  void GetVersion();
};
} // namespace BombOS
