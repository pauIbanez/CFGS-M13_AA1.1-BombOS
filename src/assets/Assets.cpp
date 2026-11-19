#include "Assets.h"
#include "Renderer.h"
#include <raylib-cpp/Sound.hpp>
#include <raylib.h>
#include <utility>
#include <vector>

namespace BombOS {

std::unordered_map<std::string, raylib::Texture2D> Assets::textures;
std::unordered_map<std::string, raylib::Model> Assets::models;
raylib::Music Assets::music;
std::unordered_map<std::string, raylib::ModelAnimation> Assets::animations;

void Assets::LoadTextures(const Map::TextureStore &textureStore) {
  textures.clear();
  for (const std::pair<std::string, std::string> &item : textureStore) {
    textures.emplace(item.first, raylib::Texture2D("./assets/textures/" + item.second));
  }
}

raylib::Texture2D &Assets::GetTexture(const std::string &name) { return textures[name]; }

void Assets::LoadModels() {
  LoadModel("bomberman", "./assets/player/bomberman.glb");
  LoadModel("bomb", "./assets/models/bomb.glb");
}

raylib::Model &Assets::GetModel(const std::string &name) { return models.at(name); }

void Assets::LoadMusic(const std::string &musicName) {
  music = raylib::Music("./assets/music/" + musicName);
}
raylib::Music &Assets::GetMusic() { return music; }


const raylib::ModelAnimation &Assets::GetAnimation(const std::string &name) {
  return animations.at(name);
}

void Assets::Unload() {
  animations.clear();
  models.clear();
  music = raylib::Music();
  textures.clear();
}

void Assets::LoadModel(const std::string &name, const std::string &path) {
  raylib::Model &model = models.emplace(name, raylib::Model(path)).first->second;


  for (int i = 0; i < model.materialCount; i++)
    model.materials[i].shader = Renderer::GetShader();

  std::vector<raylib::ModelAnimation> clips = raylib::ModelAnimation::Load(path);
  for (raylib::ModelAnimation &clip : clips) {
    std::string clipName = clip.name;
    animations.emplace(clipName, std::move(clip));
  }
}
} // namespace BombOS
