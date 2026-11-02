#pragma once

#include "Player.h"
#include "WindowManager.h"
#include <memory>
#include <raylib-cpp.hpp>

namespace BombOS {
class Renderer {
private:
  static std::unique_ptr<raylib::Shader> shader;

  static const raylib::Vector3 lightDir;
  static const float lightDistance;
  static const float shadowStrength;
  static const int shadowMapSize;
  static const Color playerShadowColor;
  static const raylib::Vector3 playerSize;

  static RenderTexture2D shadowMap;
  static Matrix lightVP;
  static int lightVPLoc;

public:
  static inline raylib::Shader &GetShader() { return *shader; }

  static void Init();
  static void Render(WindowManager &window, raylib::Camera3D &camera);
  static void Shutdown();

  // Grid space (x = column, y = row) to world space, height being the world Y
  static raylib::Vector3 GridToWorld(const raylib::Vector2 &cell, float height = 0);

  static void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height,
                              float length, Color color);

  static void DrawPlaneTexture(Texture2D texture, Vector3 position, float width, float length,
                               Color tint);

  static void DrawShadowDisc(Vector3 center, float radius, Color color);

private:
  static void DrawWorld(bool drawFloor);
  static void DrawPlayer(const Player &player);
  static void RenderShadowMap();
  static RenderTexture2D LoadShadowMap(int size);
  static void UnloadShadowMap(RenderTexture2D target);
};
} // namespace BombOS
