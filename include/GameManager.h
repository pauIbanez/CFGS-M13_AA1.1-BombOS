#pragma once

#include "MapData.h"
#include "Player.h"
#include "WindowManager.h"
#include <memory>
#include <raylib-cpp.hpp>
#include <raylib-cpp/Texture.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace BombOS {
class GameManager {
public:
  static bool isPaused;

  static inline WindowManager &GetWindow() { return *window; }
  static inline raylib::Camera3D &GetCamera() { return camera; }
  static inline raylib::Shader &GetShader() { return *shader; }
  static inline Map::MapData &GetMapData() { return *mapData; }

private:
  static bool isHalted;

  static std::unique_ptr<WindowManager> window;
  static raylib::Camera3D camera;
  static std::unique_ptr<raylib::Shader> shader;

  static const raylib::Vector3 lightDir;
  static const float lightDistance;
  static const float shadowStrength;
  static const int shadowMapSize;
  static const Color playerShadowColor;

  static RenderTexture2D shadowMap;
  static Matrix lightVP;
  static int lightVPLoc;

  static std::unique_ptr<Map::MapData> loadedMap;
  static std::unique_ptr<Map::MapData> mapData;
  static raylib::Vector3 offset;
  static std::vector<std::unique_ptr<Player>> players;
  static std::unordered_map<std::string, raylib::Texture2D> textures;

public:
  static void Init();
  static void Update();
  static void Render();
  static void Pause();
  static void Resume();
  static void LoadLevel(const std::string &levelName);
  static void StartLevel();
  static void SpawnPlayers();
  static void Shutdown();

  static bool ShouldClose();

  static void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height,
                              float length, Color color);

  static void DrawPlaneTexture(Texture2D texture, Vector3 position, float width, float length,
                               Color tint);

  static void DrawShadowDisc(Vector3 center, float radius, Color color);

private:
  static void DrawWorld(bool drawFloor);
  static void RenderShadowMap();
  static RenderTexture2D LoadShadowMap(int size);
  static void UnloadShadowMap(RenderTexture2D target);
};
} // namespace BombOS
