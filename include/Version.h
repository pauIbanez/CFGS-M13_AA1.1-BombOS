#pragma once


#include <string>
namespace BombOS {

class Version {

public:
  Version(short major, short minor, short patch) : major(major), minor(minor), patch(patch) {};

public:
  const short GetMajor();
  const short GetMinor();
  const short GetPath();

private:
  short major;
  short minor;
  short patch;

public:
  const std::string ToString() const;


public:
  bool operator==(const Version &o) const {
    return major == o.major && minor == o.minor && patch == o.patch;
  }
  bool operator!=(const Version &o) const { return !(*this == o); }
  bool operator<(const Version &o) const {
    if (major != o.major) return major < o.major;
    if (minor != o.minor) return minor < o.minor;
    return patch < o.patch;
  }
  bool operator>(const Version &o) const { return o < *this; }
};
} // namespace BombOS
