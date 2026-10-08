
#include "reader.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

namespace BombOS {

char Reader::delimeter = ';';
std::string Reader::header("BDN");

Reader::~Reader() {
  if (stream && stream.is_open()) stream.close();
}

void Reader::Read() {
  stream = std::ifstream(filePath);
  if (!stream) {
    printf("Could not open %s\n", filePath.c_str());
    return;
  }


  if (!IsValidFormat()) {
    printf("Invalid header!");
    return;
  }
  GetVersion();
  printf("done!");
}

bool Reader::IsValidFormat() {
  std::string readHeader;
  std::getline(stream, readHeader, delimeter);
  return readHeader == header;
}

void Reader::GetVersion() { std::getline(stream, version, delimeter); }

} // namespace BombOS
