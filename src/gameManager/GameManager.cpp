#include "GameManager.h"
#include "Assets.h"
#include "Level.h"
#include "Renderer.h"
#include <raylib-cpp/Keyboard.hpp>
#include <raylib.h>

namespace BombOS {

bool GameManager::isPaused = false;
bool GameManager::isHalted = false;

std::unique_ptr<WindowManager> GameManager::window;
raylib::Camera3D GameManager::camera;

void GameManager::Init() {
  window = std::make_unique<WindowManager>();

  camera = raylib::Camera3D({-7.5f, 14.0f, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f,
                            CAMERA_PERSPECTIVE);

  Renderer::Init();

  LoadLevel("./assets/level4.bos");
}

void GameManager::Update() {
  if (isHalted) return;
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsCursorHidden()) DisableCursor();
  if (!IsWindowFocused() && IsCursorHidden()) EnableCursor();


  if (raylib::Keyboard::IsKeyPressed(KEY_ESCAPE)) EnableCursor();
  if (raylib::Keyboard::IsKeyPressed(KEY_R)) Level::Start();

  Level::Update();
}

void GameManager::Render() { Renderer::Render(*window, camera); }

void GameManager::Pause() {}
void GameManager::Resume() {}

void GameManager::LoadLevel(const std::string &levelName) {
  isHalted = true;
  Level::Load(levelName);
  isHalted = false;
}

void GameManager::Shutdown() {
  Level::Unload();
  Assets::Unload();
  Renderer::Shutdown();
  window.reset();
}

bool GameManager::ShouldClose() { return window->ShouldClose(); }
} // namespace BombOS
