#include "GameManager.h"
#include <memory>
#define RLIGHTS_IMPLEMENTATION
#include "Reader.h"
#include "rlights.h"
#include <cstddef>
#include <raylib-cpp/Keyboard.hpp>

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

void GameManager::Init() {
  // The window has to exist before anything that touches the GPU (shaders, lights...)
  window = std::make_unique<WindowManager>();

  camera = raylib::Camera3D({-7.5f, 14.0f, 0},  // position
                            {0.0f, 0.0f, 0.0f}, // target
                            {0.0f, 1.0f, 0.0f}, // up
                            45.0f, CAMERA_PERSPECTIVE);

  shader = std::make_unique<raylib::Shader>("resources/lighting.vs", "resources/lighting.fs");
  shader->locs[SHADER_LOC_VECTOR_VIEW] = shader->GetLocation("viewPos");

  float ambient[4] = {0.5f, 0.5f, 0.5f, 1}; // so the dark side isn't pitch black
  shader->SetValue(shader->GetLocation("ambient"), ambient, SHADER_UNIFORM_VEC4);

  sun = CreateLight(LIGHT_DIRECTIONAL, {-10, 20, 5}, {0, 0, 0}, WHITE, *shader);

  player = std::make_unique<Player>();

  LoadLevel("./assets/level1.bos");
}

void GameManager::Update() {
  if (isHalted) return;
  // Lock the mouse only after clicking the window, so it can't grab it while hidden behind other apps
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
    float x = i / mapData->mapSize.height;
    float y = i % mapData->mapSize.width;
    DrawPlane((raylib::Vector3{x, 0, y} + offset), {1, 1}, RED);
    if (mapData->main.At(x, y) != "0")
      DrawCube((raylib::Vector3{x, 0.5f, y} + offset), 1, 1, 1, BLUE);
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

  StartLevel();
  isHalted = false;
}

void GameManager::StartLevel() {}

void GameManager::Shutdown() {
  // GPU resources must be released before the window (and its GL context) closes
  mapData.reset();
  shader.reset();
  window.reset();
}

bool GameManager::ShouldClose() { return window->ShouldClose(); }
} // namespace BombOS
