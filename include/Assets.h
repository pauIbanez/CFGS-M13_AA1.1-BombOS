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
  static std::unordered_map<std::string, raylib::Model> models;
  static raylib::Music music;
  static std::unordered_map<std::string, raylib::ModelAnimation> animations;

public:
  static void LoadTextures(const std::string &folder, const Map::TextureStore &textureStore);
  static raylib::Texture2D &GetTexture(const std::string &name);

  static void LoadModels();
  static raylib::Model &GetModel(const std::string &name);

  static void LoadMusic(const std::string &folder, const std::string &musicName);
  static raylib::Music &GetMusic();

  static const raylib::ModelAnimation &GetAnimation(const std::string &name);

  static void Unload();

private:
  static void LoadModel(const std::string &name, const std::string &path);
};
} // namespace BombOS
