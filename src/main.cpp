#include "Reader.h"
#include "WindowManager.h"
#include <cstdio>
#include <exception>


using namespace BombOS;

int main() {
  Reader *reader = new Reader("./assets/level1.bos");

  try {
    reader->Read();
    WindowManager::Init();


    // Camera camera = {};
    // camera.position = {16.0f, 14.0f, 16.0f};
    // camera.target = {0.0f, 0.0f, 0.0f};
    // camera.up = {0.0f, 1.0f, 0.0f}; // currently missing — needed for the camera to render correctly
    // camera.fovy = 45.0f;
    // camera.projection = CAMERA_PERSPECTIVE;
    // bool pause = false;

    while (!WindowManager::ShouldClose()) {
      // Update
      // if (IsKeyPressed(KEY_P)) pause = !pause;
      // if (!pause) UpdateCamera(&camera, CAMERA_ORBITAL);

      // // Draw
      WindowManager::BeginFrame();
      // BeginMode3D(camera);
      // // DrawModel(...)
      // EndMode3D();
      WindowManager::EndFrame();
    }

    WindowManager::Shutdown();



  } catch (const std::exception &e) {
    printf("BOSError: %s\n", e.what());
  }

  delete reader;
  printf("\nPress Enter to exit...");
}
