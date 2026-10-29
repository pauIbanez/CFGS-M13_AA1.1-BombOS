#include "GameManager.h"
#include <memory>
#include <raylib-cpp/Texture.hpp>
#include <raylib.h>
#include <utility>
#define RLIGHTS_IMPLEMENTATION
#include "Reader.h"
#include "rlights.h"
#include <cstddef>
#include <raylib-cpp/Keyboard.hpp>
#include <rlgl.h>
namespace BombOS {

bool GameManager::isPaused = false;
bool GameManager::isHalted = false;

std::unique_ptr<WindowManager> GameManager::window;
raylib::Camera3D GameManager::camera;
std::unique_ptr<raylib::Shader> GameManager::shader;
Light GameManager::sun;

std::unique_ptr<Map::MapData> GameManager::mapData;
raylib::Vector3 GameManager::offset;

std::unique_ptr<Player> GameManager::player;
std::unordered_map<std::string, raylib::Texture2D> GameManager::textures;

void GameManager::Init() {
  window = std::make_unique<WindowManager>();

  camera = raylib::Camera3D({-7.5f, 14.0f, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f,
                            CAMERA_PERSPECTIVE);

  shader = std::make_unique<raylib::Shader>("resources/lighting.vs", "resources/lighting.fs");
  shader->locs[SHADER_LOC_VECTOR_VIEW] = shader->GetLocation("viewPos");

  float ambient[4] = {0.5f, 0.5f, 0.5f, 1};
  shader->SetValue(shader->GetLocation("ambient"), ambient, SHADER_UNIFORM_VEC4);

  sun = CreateLight(LIGHT_DIRECTIONAL, {-10, 20, 5}, {0, 0, 0}, WHITE, *shader);

  player = std::make_unique<Player>();

  LoadLevel("./assets/level3.bos");
}

void GameManager::Update() {
  if (isHalted) return;
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsCursorHidden()) DisableCursor();
  if (!IsWindowFocused() && IsCursorHidden()) EnableCursor();


  if (raylib::Keyboard::IsKeyPressed(KEY_ESCAPE)) EnableCursor();

  player->Update();
}

void GameManager::Render() {
  window->BeginFrame();

  float camPos[3] = {camera.position.x, camera.position.y, camera.position.z};
  shader->SetValue(shader->locs[SHADER_LOC_VECTOR_VIEW], camPos, SHADER_UNIFORM_VEC3);
  UpdateLightValues(*shader, sun);

  camera.BeginMode();
  shader->BeginMode();


  for (size_t i = 0; i < mapData->mapSize.width * mapData->mapSize.height; i++) {
    float x = i % mapData->mapSize.width;
    float y = (float)i / mapData->mapSize.width;

    const std::string &floor = mapData->floor.At(x, y);
    if (floor != "0") {
      const raylib::Texture2D &texture = textures[floor];
      DrawPlaneTexture(texture, (raylib::Vector3{x, 0.5f, y} + offset), 1, 1, WHITE);
    }

    const std::string &main = mapData->main.At(x, y);
    if (main != "0") {
      const raylib::Texture2D &texture = textures[main];
      DrawCubeTexture(texture, (raylib::Vector3{x, 0.5f, y} + offset), 1, 1, 1, WHITE);
    }
  }
  player->Render();


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

  offset = raylib::Vector3{-(mapData->mapSize.width - 1) * 0.5f, 0,
                           -(mapData->mapSize.height - 1) * 0.5f};

  textures.clear();
  for (const std::pair<std::string, std::string> &item : mapData->textureStore) {
    textures.emplace(item.first, raylib::Texture2D("./assets/textures/" + item.second));
  }

  StartLevel();
  isHalted = false;
}

void GameManager::StartLevel() {}

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
} // namespace BombOS
