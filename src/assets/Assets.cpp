#include "Assets.h"
#include <utility>

namespace BombOS {

std::unordered_map<std::string, raylib::Texture2D> Assets::textures;

void Assets::LoadTextures(const Map::TextureStore &textureStore) {
  textures.clear();
  for (const std::pair<std::string, std::string> &item : textureStore) {
    textures.emplace(item.first, raylib::Texture2D("./assets/textures/" + item.second));
  }
}

raylib::Texture2D &Assets::GetTexture(const std::string &name) { return textures[name]; }

void Assets::Unload() { textures.clear(); }
} // namespace BombOS
