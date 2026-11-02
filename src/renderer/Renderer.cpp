#include "Renderer.h"
#include "Assets.h"
#include "Level.h"
#include <cmath>
#include <cstddef>
#include <raylib-cpp/Vector3.hpp>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

namespace BombOS {

std::unique_ptr<raylib::Shader> Renderer::shader;

const raylib::Vector3 Renderer::lightDir = raylib::Vector3{0.25f, -1.0f, 0.45f}.Normalize();
const float Renderer::lightDistance = 20.0f;
const float Renderer::shadowStrength = 0.65f;
const int Renderer::shadowMapSize = 2048;
const Color Renderer::playerShadowColor{0, 0, 0, 70};
const float Renderer::playerScale = 0.65f;
const float Renderer::animationFps = 60;

raylib::Model *Renderer::playerModel = nullptr;
std::array<const raylib::ModelAnimation *, (size_t)Player::Animation::Count>
    Renderer::playerAnimations{};

RenderTexture2D Renderer::shadowMap{};
Matrix Renderer::lightVP{};
int Renderer::lightVPLoc = -1;

void Renderer::Init() {
  shader = std::make_unique<raylib::Shader>("resources/shading.vs", "resources/shading.fs");

  float shadePos[3] = {0.85f, 1.0f, 0.75f};
  float shadeNeg[3] = {0.85f, 0.5f, 0.75f};
  shader->SetValue(shader->GetLocation("shadePos"), shadePos, SHADER_UNIFORM_VEC3);
  shader->SetValue(shader->GetLocation("shadeNeg"), shadeNeg, SHADER_UNIFORM_VEC3);

  shadowMap = LoadShadowMap(shadowMapSize);
  int shadowMapSlot = 10;
  shader->SetValue(shader->GetLocation("shadowMap"), &shadowMapSlot, SHADER_UNIFORM_INT);
  shader->SetValue(shader->GetLocation("lightDir"), &lightDir, SHADER_UNIFORM_VEC3);
  shader->SetValue(shader->GetLocation("shadowStrength"), &shadowStrength, SHADER_UNIFORM_FLOAT);
  lightVPLoc = shader->GetLocation("lightVP");


  Assets::LoadModels();
  playerModel = &Assets::GetModel("bomberman");
  playerAnimations[(size_t)Player::Animation::Idle] = &Assets::GetAnimation("idle");
  playerAnimations[(size_t)Player::Animation::Run] = &Assets::GetAnimation("run");
  playerAnimations[(size_t)Player::Animation::Victory] = &Assets::GetAnimation("victory");
}

void Renderer::Render(WindowManager &window, raylib::Camera3D &camera) {
  RenderShadowMap();

  window.BeginFrame();

  camera.BeginMode();
  shader->BeginMode();

  shader->SetValue(lightVPLoc, lightVP);
  rlActiveTextureSlot(10);
  rlEnableTexture(shadowMap.depth.id);
  rlActiveTextureSlot(0);

  DrawWorld(true);

  shader->EndMode();

  rlActiveTextureSlot(10);
  rlDisableTexture();
  rlActiveTextureSlot(0);

  camera.EndMode();
  window.EndFrame();
}

void Renderer::Shutdown() {
  UnloadShadowMap(shadowMap);
  shadowMap = {};
  playerModel = nullptr;
  playerAnimations = {};
  shader.reset();
}

raylib::Vector3 Renderer::GridToWorld(const raylib::Vector2 &cell, float height) {
  const Map::MapSize &mapSize = Level::GetMapData().mapSize;

  // Rows run along -X and columns along +Z, centered so the middle of the map sits at the origin
  return {(mapSize.height - 1) * 0.5f - cell.y, height, cell.x - (mapSize.width - 1) * 0.5f};
}

void Renderer::DrawWorld(bool drawFloor) {
  const Map::MapData &mapData = Level::GetMapData();

  for (size_t i = 0; i < mapData.mapSize.width * mapData.mapSize.height; i++) {
    size_t x = i % mapData.mapSize.width;
    size_t y = i / mapData.mapSize.width;

    raylib::Vector2 cell{(float)x, (float)y};

    const std::string &floor = mapData.floor.Read(x, y);
    if (drawFloor && floor != "0") {
      const raylib::Texture2D &texture = Assets::GetTexture(floor);
      DrawPlaneTexture(texture, GridToWorld(cell, 0), 1, 1, WHITE);
    }

    const std::string &main = mapData.main.Read(x, y);
    if (main != "0") {
      const raylib::Texture2D &texture = Assets::GetTexture(main);
      DrawCubeTexture(texture, GridToWorld(cell, 0.5f), 1, 1, 1, WHITE);
    }
  }

  for (const std::unique_ptr<Player> &player : Level::GetPlayers())
    DrawPlayer(*player);
}

void Renderer::DrawPlayer(const Player &player) {
  const raylib::ModelAnimation &animation = *playerAnimations[(size_t)player.animation];

  // Every player shares the model, so pose it right before drawing each one
  int frame = (int)(player.animationTime * animationFps) % animation.frameCount;
  UpdateModelAnimation(*playerModel, animation, frame);

  // Grid (x, y) maps to world (-y, x) on the XZ plane, and the model looks down +Z
  float yaw = atan2f(-player.facing.y, player.facing.x) * RAD2DEG;

  playerModel->Draw(GridToWorld(player.position), {0, 1, 0}, yaw,
                    raylib::Vector3::One() * playerScale);
  DrawShadowDisc(GridToWorld(player.position, 0.01f), 0.3f, playerShadowColor);
}

void Renderer::RenderShadowMap() {
  const Map::MapData &mapData = Level::GetMapData();

  float width = (float)mapData.mapSize.width;
  float height = (float)mapData.mapSize.height;
  float extent = sqrtf(width * width + height * height) * 0.5f + 2.0f;

  Matrix view = MatrixLookAt(lightDir * -lightDistance, {0, 0, 0}, {0, 1, 0});
  Matrix proj = MatrixOrtho(-extent, extent, -extent, extent, 0.1, lightDistance * 2.0f);
  lightVP = MatrixMultiply(view, proj);

  BeginTextureMode(shadowMap);
  ClearBackground(WHITE);

  rlSetMatrixProjection(proj);
  rlSetMatrixModelview(view);
  rlEnableDepthTest();


  DrawWorld(false);

  rlDrawRenderBatchActive();
  EndTextureMode();
}

void Renderer::DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height,
                               float length, Color color) {
  float x = position.x;
  float y = position.y;
  float z = position.z;

  // Set desired texture to be enabled while drawing following vertex data
  rlSetTexture(texture.id);

  rlBegin(RL_QUADS);
  rlColor4ub(color.r, color.g, color.b, color.a);
  // Front Face
  rlNormal3f(0.0f, 0.0f, 1.0f); // Normal Pointing Towards Viewer
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Left Of The Texture and Quad
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left Of The Texture and Quad
  // Back Face
  rlNormal3f(0.0f, 0.0f, -1.0f); // Normal Pointing Away From Viewer
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Left Of The Texture and Quad
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Left Of The Texture and Quad
  // Top Face
  rlNormal3f(0.0f, 1.0f, 0.0f); // Normal Pointing Up
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left Of The Texture and Quad
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Bottom Left Of The Texture and Quad
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right Of The Texture and Quad
  // Bottom Face
  rlNormal3f(0.0f, -1.0f, 0.0f); // Normal Pointing Down
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Top Left Of The Texture and Quad
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Left Of The Texture and Quad
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Right Of The Texture and Quad
  // Right face
  rlNormal3f(1.0f, 0.0f, 0.0f); // Normal Pointing Right
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Left Of The Texture and Quad
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Left Of The Texture and Quad
  // Left Face
  rlNormal3f(-1.0f, 0.0f, 0.0f); // Normal Pointing Left
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Left Of The Texture and Quad
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left Of The Texture and Quad
  rlEnd();
  //rlPopMatrix();

  rlSetTexture(0);
}
void Renderer::DrawPlaneTexture(Texture2D texture, Vector3 position, float width, float length,
                                Color tint) {
  float x = position.x, y = position.y, z = position.z;

  rlSetTexture(texture.id);
  rlBegin(RL_QUADS);
  rlColor4ub(tint.r, tint.g, tint.b, tint.a);
  rlNormal3f(0.0f, 1.0f, 0.0f);

  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x - width / 2, y, z - length / 2);
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x - width / 2, y, z + length / 2);
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x + width / 2, y, z + length / 2);
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x + width / 2, y, z - length / 2);
  rlEnd();
  rlSetTexture(0);
}
void Renderer::DrawShadowDisc(Vector3 center, float radius, Color color) {
  constexpr int segments = 20;
  constexpr float step = 2.0f * PI / segments;

  rlBegin(RL_TRIANGLES);
  rlColor4ub(color.r, color.g, color.b, color.a);
  rlNormal3f(0.0f, 1.0f, 0.0f);

  // Wound counter-clockwise seen from above so backface culling keeps it
  for (int i = 0; i < segments; i++) {
    float a = i * step;
    rlVertex3f(center.x, center.y, center.z);
    rlVertex3f(center.x + cosf(a + step) * radius, center.y, center.z + sinf(a + step) * radius);
    rlVertex3f(center.x + cosf(a) * radius, center.y, center.z + sinf(a) * radius);
  }
  rlEnd();
}

RenderTexture2D Renderer::LoadShadowMap(int size) {
  RenderTexture2D target{};
  target.id = rlLoadFramebuffer();
  target.texture.width = size;
  target.texture.height = size;

  if (target.id > 0) {
    rlEnableFramebuffer(target.id);

    target.depth.id = rlLoadTextureDepth(size, size, false);
    target.depth.width = size;
    target.depth.height = size;
    target.depth.format = 19; // DEPTH_COMPONENT_24BIT
    target.depth.mipmaps = 1;
    rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D,
                        0);

    if (!rlFramebufferComplete(target.id))
      TraceLog(LOG_WARNING, "Shadow map framebuffer incomplete");
    rlDisableFramebuffer();
  }

  return target;
}

void Renderer::UnloadShadowMap(RenderTexture2D target) {
  if (target.id == 0) return;
  rlUnloadFramebuffer(target.id); // Also unloads the attached depth texture
}
} // namespace BombOS
