#pragma once

#include <string>
#include <unordered_map>

namespace BombOS {

enum Format { BOS, PBOS };
using TextureMap = std::unordered_map<std::string, std::string>;

} // namespace BombOS
