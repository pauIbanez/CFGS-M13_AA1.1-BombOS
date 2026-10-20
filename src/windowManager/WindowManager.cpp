#include "WindowManager.h"
#include "AppConfig.h"
#include "raylib.h"

namespace BombOS {

void WindowManager::Init() {
  InitWindow(screenWidth, screenHeight, APP_NAME);
  SetTargetFPS(60);
}
void WindowManager::Shutdown() { CloseWindow(); }
bool WindowManager::ShouldClose() { return WindowShouldClose(); }
void WindowManager::BeginFrame() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
}
void WindowManager::EndFrame() {
  DrawFPS(10, 10);
  EndDrawing();
}
} // namespace BombOS
