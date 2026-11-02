#pragma once

#include "Types.h"
#include <raylib-cpp.hpp>
#include <raylib-cpp/Texture.hpp>
#include <string>
#include <unordered_map>

namespace BombOS {
class Assets {
private:
  static std::unordered_map<std::string, raylib::Texture2D> textures;

public:
  static void LoadTextures(const Map::TextureStore &textureStore);
  static raylib::Texture2D &GetTexture(const std::string &name);

  static void Unload();
};
} // namespace BombOS
