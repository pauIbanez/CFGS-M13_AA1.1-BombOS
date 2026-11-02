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
#include <rlgl.h>
#include <utility>

namespace BombOS {

bool GameManager::isPaused = false;
bool GameManager::isHalted = false;

std::unique_ptr<WindowManager> GameManager::window;
raylib::Camera3D GameManager::camera;
std::unique_ptr<raylib::Shader> GameManager::shader;

const Color GameManager::shadowColor{0, 0, 0, 70};
const float GameManager::shadowLength = 0.6f;
const float GameManager::shadowHeight = 0.51f;

std::unique_ptr<Map::MapData> GameManager::mapData;
raylib::Vector3 GameManager::offset;

std::vector<std::unique_ptr<Player>> GameManager::players;
std::unordered_map<std::string, raylib::Texture2D> GameManager::textures;

void GameManager::Init() {
  window = std::make_unique<WindowManager>();

  camera = raylib::Camera3D({-7.5f, 14.0f, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f,
                            CAMERA_PERSPECTIVE);

  shader = std::make_unique<raylib::Shader>("resources/shading.vs", "resources/shading.fs");

  // X sides, top, Z sides, bottom
  float faceShade[4] = {0.85f, 1.0f, 0.75f, 0.5f};
  shader->SetValue(shader->GetLocation("faceShade"), faceShade, SHADER_UNIFORM_VEC4);

  LoadLevel("./assets/level1.bos");
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
  window->BeginFrame();

  camera.BeginMode();
  shader->BeginMode();



  for (size_t i = 0; i < mapData->mapSize.width * mapData->mapSize.height; i++) {
    size_t x = i % mapData->mapSize.width;
    size_t y = i / mapData->mapSize.width;

    raylib::Vector3 pos = raylib::Vector3{-(float)y, 0.5f, (float)x} + offset;

    const std::string &floor = mapData->floor.At(x, y);
    if (floor != "0") {
      const raylib::Texture2D &texture = textures[floor];
      DrawPlaneTexture(texture, pos, 1, 1, WHITE);
    }

    const std::string &main = mapData->main.At(x, y);
    if (main != "0") {
      const raylib::Texture2D &texture = textures[main];
      DrawCubeTexture(texture, pos, 1, 1, 1, WHITE);
    }
  }

  // Fake shadows: drawn after the floor so they blend over it
  for (auto [x, y, main] : mapData->main.GetWithIndex()) {
    if (main == "0" || y + 1 >= mapData->mapSize.height) continue;
    if (mapData->floor.Read(x, y + 1) == "0" || mapData->main.Read(x, y + 1) != "0") continue;

    // Next row is toward the camera; shadow starts at the edge touching the wall
    raylib::Vector3 tile = raylib::Vector3{-(float)(y + 1), shadowHeight, (float)x} + offset;
    DrawPlane({tile.x + 0.5f - shadowLength * 0.5f, tile.y, tile.z}, {shadowLength, 1}, shadowColor);
  }

  for (const std::unique_ptr<Player> &player : players) {
    raylib::Vector3 pos = player->position + offset;
    DrawShadowDisc({pos.x, shadowHeight, pos.z}, Player::size.x * 0.6f, shadowColor);
  }

  for (const std::unique_ptr<Player> &player : players)
    player->Render(offset);


  shader->EndMode();
  camera.EndMode();
  window->EndFrame();
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

void GameManager::DrawShadowDisc(Vector3 center, float radius, Color color) {
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
} // namespace BombOS
