
#include "reader.h"
#include "Version.h"
#include <cstddef>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace BombOS {

const char Reader::delimeter = ';';
const std::unordered_map<std::string, Format> Reader::formatHeaders = {
    {"BOS", Format::BOS},
    {"PBOS", Format::PBOS},
};

const Version Reader::PBOSlatest = Version(0, 0, 1);
const Version Reader::BOSlatest = Version(0, 0, 5);



Reader::~Reader() {
  if (stream && stream.is_open()) stream.close();
  delete version;
}

void Reader::Read() {
  stream = std::ifstream(filePath);
  if (!stream) {
    throw std::exception(("Could not open file: " + filePath).c_str());
  }

  ReadFormat();
  ReadVersion();
  ExpectLineEnd("Version");
  ReadTitle();
  ExpectLineEnd("Title");
  ReadTextures();

  printf("done!\n");
}


void Reader::ReadFormat() {
  std::string formatStr = GetNext();

  if (!TryParseFormat(formatStr, format)) {
    throw std::exception(("Invalid format found " + formatStr).c_str());
  }

  printf("Reading format %s\n", formatStr.c_str());
}

void Reader::ReadVersion() {
  std::string versionStr = GetNext();

  if (!TryParseVersion(versionStr, version)) {
    throw std::exception(("Invalid version found " + versionStr).c_str());
  }


  switch (format) {
  case Format::BOS:
    if (*version > BOSlatest) {
      throw std::exception(("Version " + version->ToString() +
                            " is unsupported! Highest BOS version is " + BOSlatest.ToString())
                               .c_str());
    }
    break;

  case Format::PBOS:
    if (*version > PBOSlatest) {
      throw std::exception(("Version " + version->ToString() +
                            " is unsupported! Highest PBOS version is " + PBOSlatest.ToString())
                               .c_str());
    }
    break;
  }

  printf("Reading version %s\n", version->ToString().c_str());
}

void Reader::ReadTitle() {

  ExpectHeader("TITLE");
  title = GetNext();

  if (title == "") throw std::exception("Can't have empty title");

  printf("TITLE: %s\n", title.c_str());
}

void Reader::ReadTextures() {
  ExpectHeader("TEXTURES");

  switch (format) {
  case Format::BOS:
    ReadTexturesBOS();
    break;

  case Format::PBOS:
    ReadTexturesPBOS();
    break;
  }

  for (auto [k, v] : textures) {
    printf("%s: %s\n", k.c_str(), v.c_str());
  }
}


void Reader::ReadTexturesBOS() {
  std::string sizeStr = GetNext();
  int size;
  try {
    size = std::stoi(sizeStr);
  } catch (const std::exception &) {
    throw std::exception("Invalid texture size");
  }

  if (size < 0) throw std::exception("Invalid texture size");

  printf("Reading %i textures\n", size);

  ExpectLineEnd("Texture size");
  for (size_t i = 0; i < size; i++) {
    std::string ident = GetNext();
    std::string textureName = GetNext();
    ExpectLineEnd(textureName);
    if (!textures.emplace(ident, textureName).second)
      throw std::exception(("Duplicate texture identifier: " + ident).c_str());
  }
}

void Reader::ReadTexturesPBOS() {

  std::string texture = GetNext();
  short counter = 0;
  while (!IsLineEnd(texture)) {
    textures.insert({std::to_string(counter), texture});
    counter++;
    texture = GetNext();
  }
}

const bool Reader::TryParseFormat(std::string &formatStr, Format &format) {

  auto it = formatHeaders.find(formatStr);
  if (it == formatHeaders.end()) {
    return false;
  }
  format = it->second;
  return true;
}

const bool Reader::TryParseVersion(std::string &versionStr, Version *&version) {
  std::vector<std::string> parts;
  std::string part = "";
  const char splitter = '.';

  for (size_t i = 0; i < versionStr.length(); i++) {
    if (versionStr[i] == splitter) {
      parts.push_back(part);
      part = "";
      continue;
    }
    part += versionStr[i];
  }
  parts.push_back(part);

  if (format == Format::PBOS && parts.size() != 3) {
    throw std::exception(("Version must be major.minor.patch, found " + versionStr).c_str());
  }
  if (format == Format::BOS && parts.size() != 2) {
    throw std::exception(("Version must be major.patch, found " + versionStr).c_str());
  }

  short numbers[3] = {0, 0, 0};
  for (size_t i = 0; i < parts.size(); i++) {
    const char *begin = parts[i].data();
    const char *end = begin + parts[i].size();
    auto [ptr, ec] = std::from_chars(begin, end, numbers[i]);
    if (ec != std::errc() || ptr != end || numbers[i] < 0) {
      throw std::exception(("Invalid version part '" + parts[i] + "' in " + versionStr).c_str());
    }
  }

  delete version;

  switch (format) {
  case Format::BOS:
    version = new Version(numbers[0], 0, numbers[1]);
    break;

  case Format::PBOS:
    version = new Version(numbers[0], numbers[1], numbers[2]);
    break;
  }
  return true;
}


const std::string Reader::GetNext(const char &del) {
  std::string next;
  std::getline(stream, next, del);

  const char *ws = "\r\n";
  size_t start = next.find_first_not_of(ws);
  if (start == std::string::npos) return "";
  size_t end = next.find_last_not_of(ws);
  return next.substr(start, end - start + 1);
}

void Reader::ExpectHeader(const std::string &header) {
  std::string next = GetNext();
  if (next != header)
    throw std::exception(("Unexpected header. Got:" + next + " Expected: " + header).c_str());
}


const bool Reader::IsLineEnd(const std::string &read) const { return read == ""; }

void Reader::ExpectLineEnd(const std::string &after) {
  std::string next = GetNext();
  if (!IsLineEnd(next))
    throw std::exception(("Expected line end after " + after + ", got: " + next).c_str());
}

} // namespace BombOS
