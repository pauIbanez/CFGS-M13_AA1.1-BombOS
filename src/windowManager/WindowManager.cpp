#include "WindowManager.h"
#include "AppConfig.h"
#include <raylib-cpp/Window.hpp>

namespace BombOS {

WindowManager::WindowManager() : window(screenWidth, screenHeight, APP_NAME) {
  window.SetTargetFPS(60);
  window.SetExitKey(KEY_NULL); // ESC no longer closes the window
}

bool WindowManager::ShouldClose() const { return window.ShouldClose(); }

void WindowManager::BeginFrame() { window.BeginDrawing().ClearBackground(RAYWHITE); }

void WindowManager::EndFrame() {
  window.DrawFPS(10, 10);
  window.EndDrawing();
}
} // namespace BombOS
