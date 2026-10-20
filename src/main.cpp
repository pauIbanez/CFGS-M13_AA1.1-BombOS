#include "FlatMap.h"
#include "MapData.h"
#include "Reader.h"
#include "WindowManager.h"
#include <cstdio>
#include <exception>
#include <memory>
#include <raylib-cpp/Keyboard.hpp>
#include <raylib-cpp/Vector2.hpp>
#include <raylib.h>


using namespace BombOS;

int main() {
  Reader *reader = new Reader("./assets/level1.bos");

  try {
    std::unique_ptr<Map::MapData> mapData = reader->Read();
    WindowManager windowManager;

    raylib::Camera3D camera({16.0f, 14.0f, 16.0f}, // position
                            {0.0f, 0.0f, 0.0f},    // target
                            {0.0f, 1.0f, 0.0f},    // up
                            45.0f, CAMERA_PERSPECTIVE);

    Map::FlatMap<raylib::Vector2> floor(mapData->mapSize.width, mapData->mapSize.height);

    const raylib::Vector2 offset{(mapData->mapSize.width - 1) * 0.5f,
                                 (mapData->mapSize.height - 1) * 0.5f};

    for (size_t y = 0; y < mapData->mapSize.height; y++) {
      for (size_t x = 0; x < mapData->mapSize.width; x++) {
        floor.At(x, y) = raylib::Vector2{(float)x, (float)y} - offset;
      }
    }

    while (!windowManager.ShouldClose()) {
      // Lock the mouse only after clicking the window, so it can't grab it while hidden behind other apps
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsCursorHidden()) DisableCursor();
      if (!IsWindowFocused() && IsCursorHidden()) EnableCursor();

      // camera.Update(CAMERA_ORBITAL);

      if (raylib::Keyboard::IsKeyPressed(KEY_ESCAPE)) EnableCursor();
      // // Draw
      windowManager.BeginFrame();
      camera.BeginMode();
      DrawGrid(10, 1.0f);
      for (raylib::Vector2 pos : floor.Get()) {
        DrawCube({pos.x, 0, pos.y}, 1, 1, 1, RED);
      }

      camera.EndMode();

      windowManager.EndFrame();
    }

  } catch (const std::exception &e) {
    printf("BOSError: %s\n", e.what());
  }

  delete reader;
  printf("\nPress Enter to exit...");
}
