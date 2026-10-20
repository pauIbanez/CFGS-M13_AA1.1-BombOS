#include "WindowManager.h"
#include "AppConfig.h"
#include <raylib-cpp/Window.hpp>

namespace BombOS {

void WindowManager::Init() {

  window = raylib::Window(screenWidth, screenHeight, APP_NAME);
  window.SetTargetFPS(60);
}

void WindowManager::BeginFrame() { window.BeginDrawing().ClearBackground(RAYWHITE); }

void WindowManager::EndFrame() {
  window.DrawFPS(10, 10);
  window.EndDrawing();
}
} // namespace BombOS
