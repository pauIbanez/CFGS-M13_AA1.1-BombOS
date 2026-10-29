#pragma once

#include "MapData.h"
#include "Player.h"
#include "WindowManager.h"
#include "rlights.h"
#include <memory>
#include <raylib-cpp.hpp>
#include <raylib-cpp/Texture.hpp>
#include <string>
#include <unordered_map>

namespace BombOS {
class GameManager {
public:
  static bool isPaused;

  static inline WindowManager &GetWindow() { return *window; }
  static inline raylib::Camera3D &GetCamera() { return camera; }
  static inline raylib::Shader &GetShader() { return *shader; }
  static inline Light &GetSun() { return sun; }
  static inline Map::MapData &GetMapData() { return *mapData; }

private:
  static bool isHalted;

  static std::unique_ptr<WindowManager> window;
  static raylib::Camera3D camera;
  static std::unique_ptr<raylib::Shader> shader;
  static Light sun;

  static std::unique_ptr<Map::MapData> mapData;
  static raylib::Vector3 offset;
  static std::unique_ptr<Player> player;
  static std::unordered_map<std::string, raylib::Texture2D> textures;

public:
  static void Init();
  static void Update();
  static void Render();
  static void Pause();
  static void Resume();
  static void LoadLevel(const std::string &levelName);
  static void StartLevel();
  static void Shutdown();

  static bool ShouldClose();

  static void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height,
                              float length, Color color);

  static void DrawPlaneTexture(Texture2D texture, Vector3 position, float width, float length,
                               Color tint);
};
} // namespace BombOS
