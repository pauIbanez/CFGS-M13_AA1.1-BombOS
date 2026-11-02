#include "GameManager.h"
#include "Reader.h"
#include <charconv>
#include <cmath>
#include <cstddef>
#include <memory>
#include <raylib-cpp/Keyboard.hpp>
#include <raylib-cpp/Texture.hpp>
#include <raylib-cpp/Vector3.hpp>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <utility>

namespace BombOS {

bool GameManager::isPaused = false;
bool GameManager::isHalted = false;

std::unique_ptr<WindowManager> GameManager::window;
raylib::Camera3D GameManager::camera;
std::unique_ptr<raylib::Shader> GameManager::shader;

const raylib::Vector3 GameManager::lightDir = raylib::Vector3{0.25f, -1.0f, 0.45f}.Normalize();
const float GameManager::lightDistance = 20.0f;
const float GameManager::shadowStrength = 0.65f;
const int GameManager::shadowMapSize = 2048;

RenderTexture2D GameManager::shadowMap{};
Matrix GameManager::lightVP{};
int GameManager::lightVPLoc = -1;

std::unique_ptr<Map::MapData> GameManager::mapData;
raylib::Vector3 GameManager::offset;

std::vector<std::unique_ptr<Player>> GameManager::players;
std::unordered_map<std::string, raylib::Texture2D> GameManager::textures;

void GameManager::Init() {
  window = std::make_unique<WindowManager>();

  camera = raylib::Camera3D({-7.5f, 14.0f, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f,
                            CAMERA_PERSPECTIVE);

  shader = std::make_unique<raylib::Shader>("resources/shading.vs", "resources/shading.fs");

  float shadePos[3] = {0.85f, 1.0f, 0.75f}; // back, top, right
  float shadeNeg[3] = {0.85f, 0.5f, 0.75f}; // front, bottom, left
  shader->SetValue(shader->GetLocation("shadePos"), shadePos, SHADER_UNIFORM_VEC3);
  shader->SetValue(shader->GetLocation("shadeNeg"), shadeNeg, SHADER_UNIFORM_VEC3);

  shadowMap = LoadShadowMap(shadowMapSize);
  int shadowMapSlot = 10; // Far from the slots rlgl's batch uses for its own textures
  shader->SetValue(shader->GetLocation("shadowMap"), &shadowMapSlot, SHADER_UNIFORM_INT);
  shader->SetValue(shader->GetLocation("lightDir"), &lightDir, SHADER_UNIFORM_VEC3);
  shader->SetValue(shader->GetLocation("shadowStrength"), &shadowStrength, SHADER_UNIFORM_FLOAT);
  lightVPLoc = shader->GetLocation("lightVP");

  LoadLevel("./assets/level4.bos");
}

void GameManager::Update() {
  if (isHalted) return;
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsCursorHidden()) DisableCursor();
  if (!IsWindowFocused() && IsCursorHidden()) EnableCursor();


  if (raylib::Keyboard::IsKeyPressed(KEY_ESCAPE)) EnableCursor();

  for (const std::unique_ptr<Player> &player : players)
    player->Update();
}

void GameManager::Render() {
  RenderShadowMap();

  window->BeginFrame();

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
  window->EndFrame();
}

void GameManager::DrawWorld(bool drawFloor) {
  for (size_t i = 0; i < mapData->mapSize.width * mapData->mapSize.height; i++) {
    size_t x = i % mapData->mapSize.width;
    size_t y = i / mapData->mapSize.width;

    raylib::Vector3 pos = raylib::Vector3{-(float)y, 0, (float)x} + offset;

    const std::string &floor = mapData->floor.At(x, y);
    if (drawFloor && floor != "0") {
      const raylib::Texture2D &texture = textures[floor];
      DrawPlaneTexture(texture, pos, 1, 1, WHITE);
    }

    pos.y = 0.5f;
    const std::string &main = mapData->main.At(x, y);
    if (main != "0") {
      const raylib::Texture2D &texture = textures[main];
      DrawCubeTexture(texture, pos, 1, 1, 1, WHITE);
    }
  }

  for (const std::unique_ptr<Player> &player : players)
    player->Render(offset);
}

void GameManager::RenderShadowMap() {
  // Orthographic box around the map as seen from the sun, wide enough for any orientation
  float width = (float)mapData->mapSize.width;
  float height = (float)mapData->mapSize.height;
  float extent = sqrtf(width * width + height * height) * 0.5f + 2.0f;

  Matrix view = MatrixLookAt(lightDir * -lightDistance, {0, 0, 0}, {0, 1, 0});
  Matrix proj = MatrixOrtho(-extent, extent, -extent, extent, 0.1, lightDistance * 2.0f);
  lightVP = MatrixMultiply(view, proj);

  BeginTextureMode(shadowMap);
  ClearBackground(WHITE);

  rlSetMatrixProjection(proj);
  rlSetMatrixModelview(view);
  rlEnableDepthTest();

  // Only casters, the floor just receives
  DrawWorld(false);

  rlDrawRenderBatchActive();
  EndTextureMode();
}

void GameManager::Pause() {}
void GameManager::Resume() {}

void GameManager::LoadLevel(const std::string &levelName) {
  isHalted = true;

  Reader reader(levelName);
  mapData = reader.Read();

  offset = raylib::Vector3{(mapData->mapSize.height - 1) * 0.5f, 0,
                           -(mapData->mapSize.width - 1) * 0.5f};

  textures.clear();
  for (const std::pair<std::string, std::string> &item : mapData->textureStore) {
    textures.emplace(item.first, raylib::Texture2D("./assets/textures/" + item.second));
  }

  StartLevel();
  isHalted = false;
}

void GameManager::SpawnPlayers() {

  for (auto [x, y, object] : mapData->objects.GetWithIndex()) {
    short player;
    const char *begin = object.data();
    const char *end = begin + object.size();
    auto [ptr, ec] = std::from_chars(begin, end, player);
    if (ec != std::errc() || ptr != end || player == 0) continue;

    raylib::Vector3 pos{-(float)x, (Player::size.y * 0.5f), (float)y};
    players.push_back(std::make_unique<Player>(player, pos));
  }
}

void GameManager::StartLevel() {
  // Reset objects
  // Reset positions

  players.clear();
  SpawnPlayers();
}

void GameManager::Shutdown() {
  mapData.reset();
  UnloadShadowMap(shadowMap);
  shader.reset();
  window.reset();
}

bool GameManager::ShouldClose() { return window->ShouldClose(); }


void GameManager::DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height,
                                  float length, Color color) {
  float x = position.x;
  float y = position.y;
  float z = position.z;

  // Set desired texture to be enabled while drawing following vertex data
  rlSetTexture(texture.id);

  // Vertex data transformation can be defined with the commented lines,
  // but in this example we calculate the transformed vertex data directly when calling rlVertex3f()
  //rlPushMatrix();
  // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
  //rlTranslatef(2.0f, 0.0f, 0.0f);
  //rlRotatef(45, 0, 1, 0);
  //rlScalef(2.0f, 2.0f, 2.0f);

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
void GameManager::DrawPlaneTexture(Texture2D texture, Vector3 position, float width, float length,
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

// Depth-only framebuffer, the color output of the shadow pass is thrown away
RenderTexture2D GameManager::LoadShadowMap(int size) {
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
    rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

    if (!rlFramebufferComplete(target.id)) TraceLog(LOG_WARNING, "Shadow map framebuffer incomplete");
    rlDisableFramebuffer();
  }

  return target;
}

void GameManager::UnloadShadowMap(RenderTexture2D target) {
  if (target.id == 0) return;
  rlUnloadFramebuffer(target.id); // Also unloads the attached depth texture
}
} // namespace BombOS
