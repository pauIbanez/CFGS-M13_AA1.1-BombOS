
#include "reader.h"
#include "Version.h"
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace BombOS {

char Reader::delimeter = ';';
std::unordered_map<std::string, Format> Reader::headers = {
    {"BOS", Format::BOS},
    {"PBOS", Format::PBOS},
};

Version *Reader::PBOSlatest = new Version(0, 0, 1);
Version *Reader::BOSlatest = new Version(0, 0, 5);

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



  printf("done!\n");
}


void Reader::ReadFormat() {
  std::string foundHeader;

  if (!TryGetFormat(format, foundHeader)) {
    throw std::exception(("Invalid header found " + foundHeader).c_str());
  }

  printf("Reading format %s\n", foundHeader.c_str());
}


void Reader::ReadVersion() {

  std::string versionStr;

  if (!TryGetVersion(version, versionStr)) {
    throw std::exception(("Invalid version found " + versionStr).c_str());
  }


  switch (format) {
  case Format::BOS:
    if (version > BOSlatest) {
      throw std::exception(("Version " + version->ToString() +
                            " is unsupported! Highest BOS version is " + BOSlatest->ToString())
                               .c_str());
    }
    break;

  case Format::PBOS:
    if (version > PBOSlatest) {
      throw std::exception(("Version " + version->ToString() +
                            " is unsupported! Highest PBOS version is " + PBOSlatest->ToString())
                               .c_str());
    }
    break;
  }



  printf("Reading version %s\n", version->ToString().c_str());
}

const bool Reader::TryGetFormat(Format &format, std::string &header) {
  header = GetNext();

  auto it = headers.find(header);
  if (it == headers.end()) {
    return false;
  }
  format = it->second;
  return true;
}

const bool Reader::TryGetVersion(Version *&version, std::string &versionStr) {
  versionStr = GetNext();

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
  if (format == Format::BOS) {
    // BOS is major.patch, so the second number is the patch
    version = new Version(numbers[0], 0, numbers[1]);
  } else {
    version = new Version(numbers[0], numbers[1], numbers[2]);
  }
  return true;
}


const std::string Reader::GetNext(const char &del) {
  std::string next;
  std::getline(stream, next, del);
  return next;
}

} // namespace BombOS
