#pragma once

#include "FlatMap.h"
#include <string>
#include <unordered_map>

namespace BombOS {

enum Format { BOS, PBOS };
namespace Map {

using StringMap = FlatMap<>;
using TextureStore = std::unordered_map<std::string, std::string>;
} // namespace Map

} // namespace BombOS
