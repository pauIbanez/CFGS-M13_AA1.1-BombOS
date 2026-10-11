

#include "Version.h"
#include <string>

namespace BombOS {

const short Version::GetMajor() { return major; }
const short Version::GetMinor() { return minor; }
const short Version::GetPath() { return patch; }

const std::string Version::ToString() const {
  return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}
} // namespace BombOS
