#include "Reader.h"
#include "WindowManager.h"
#include <cstdio>
#include <exception>
#include <raylib-cpp/Keyboard.hpp>
#include <raylib.h>


using namespace BombOS;

int main() {
  Reader *reader = new Reader("./assets/level1.bos");

  try {
    reader->Read();
    WindowManager windowManager;

    raylib::Camera3D camera({16.0f, 14.0f, 16.0f}, // position
                            {0.0f, 0.0f, 0.0f},    // target
                            {0.0f, 1.0f, 0.0f},    // up
                            45.0f, CAMERA_PERSPECTIVE);

    while (!windowManager.ShouldClose()) {
      // Lock the mouse only after clicking the window, so it can't grab it while hidden behind other apps
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsCursorHidden()) DisableCursor();
      if (!IsWindowFocused() && IsCursorHidden()) EnableCursor();

      // Update
      // if (IsKeyPressed(KEY_P)) pause = !pause;
      // if (!pause) UpdateCamera(&camera, CAMERA_ORBITAL);
      camera.Update(CAMERA_ORBITAL);

      if (raylib::Keyboard::IsKeyPressed(KEY_ESCAPE)) EnableCursor();
      // // Draw
      windowManager.BeginFrame();
      camera.BeginMode();
      DrawGrid(10, 1.0f);
      DrawCube({0, 0, 0}, 2, 2, 2, RED);
      // model.Draw({0,0,0});   // raylib::Model
      camera.EndMode();

      windowManager.EndFrame();
    }

  } catch (const std::exception &e) {
    printf("BOSError: %s\n", e.what());
  }

  delete reader;
  printf("\nPress Enter to exit...");
}
