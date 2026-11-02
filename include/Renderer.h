#pragma once

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

  static RenderTexture2D shadowMap;
  static Matrix lightVP;
  static int lightVPLoc;

public:
  static inline raylib::Shader &GetShader() { return *shader; }

  static void Init();
  static void Render(WindowManager &window, raylib::Camera3D &camera);
  static void Shutdown();

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
