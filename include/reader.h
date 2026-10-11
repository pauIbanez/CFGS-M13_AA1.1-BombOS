#pragma once


#include <fstream>
#include <map>
#include <string>
namespace BombOS {

enum Format { BOS, PBOS };

class Reader {
private:
  static char delimeter;
  static std::map<Format, std::string> headers;

private:
  Format fileFormat;
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
