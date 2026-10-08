
#include "reader.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

namespace BombOS {

char Reader::delimeter = ';';
std::string Reader::header("BOS");

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

    return;
  }
  GetVersion();
  printf("done!");
}

std::string Reader::GetNext() {
  std::string next;
  std::getline(stream, next, delimeter);
  return next;
}

bool Reader::IsValidFormat() {
  std::string readHeader = GetNext();

  if (readHeader != header) {
    printf("Invalid header: %s", readHeader.c_str());
    return false;
  }
  return true;
}

void Reader::GetVersion() { version = *GetNext(); }

} // namespace BombOS
