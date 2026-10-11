
#include "reader.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

namespace BombOS {

char Reader::delimeter = ';';
std::unordered_map<std::string, Format> Reader::headers = {
    {"BOS", Format::BOS},
    {"PBOS", Format::PBOS},
};

Reader::~Reader() {
  if (stream && stream.is_open()) stream.close();
}

void Reader::Read() {
  stream = std::ifstream(filePath);
  if (!stream) {
    printf("Could not open %s\n", filePath.c_str());
    return;
  }


  if (!IsValidFormat(Format & foundFormat)) {

    return;
  }
  GetVersion();
  printf("done!");
}

const std::string Reader::GetNext() {
  std::string next;
  std::getline(stream, next, delimeter);
  return next;
}

const bool Reader::IsValidFormat(Format &foundFormat) {
  std::string readHeader = GetNext();

  auto it = headers.find(readHeader);
  if (it == headers.end()) {
    printf("Invalid header: %s", readHeader.c_str());
    return false;
  }
  foundFormat = it->second;
  return true;
}

void Reader::GetVersion() { version = GetNext(); }

} // namespace BombOS
