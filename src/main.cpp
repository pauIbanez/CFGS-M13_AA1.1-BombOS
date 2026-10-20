#define RLIGHTS_IMPLEMENTATION

#include "FlatMap.h"
#include "MapData.h"
#include "Reader.h"
#include "WindowManager.h"
#include "rlights.h"
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

    raylib::Camera3D camera({7.5f, 14.0f, 0},   // position
                            {0.0f, 0.0f, 0.0f}, // target
                            {0.0f, 1.0f, 0.0f}, // up
                            45.0f, CAMERA_PERSPECTIVE);

    Map::FlatMap<raylib::Vector2> floor(mapData->mapSize.width, mapData->mapSize.height);
    Map::FlatMap<raylib::Vector2 *> map(mapData->mapSize.width, mapData->mapSize.height);

    const raylib::Vector2 offset{(mapData->mapSize.width - 1) * 0.5f,
                                 (mapData->mapSize.height - 1) * 0.5f};

    for (size_t y = 0; y < mapData->mapSize.height; y++) {
      for (size_t x = 0; x < mapData->mapSize.width; x++) {
        floor.At(x, y) = raylib::Vector2{(float)x, (float)y} - offset;
      }
    }

    for (size_t y = 0; y < mapData->mapSize.height; y++) {
      for (size_t x = 0; x < mapData->mapSize.width; x++) {
        raylib::Vector2 *position = nullptr;
        printf("Map %d %d is %s\n", x, y, mapData->main.Read(x, y).c_str());
        if (mapData->main.Read(x, y) != "0") {
          position = new raylib::Vector2(raylib::Vector2{(float)x, (float)y} - offset);
        }
        map.At(x, y) = position;
      }
    }

    raylib::Shader shader("resources/lighting.vs", "resources/lighting.fs");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = shader.GetLocation("viewPos");

    float ambient[4] = {0.5f, 0.5f, 0.5f, 1}; // so the dark side isn't pitch black
    shader.SetValue(shader.GetLocation("ambient"), ambient, SHADER_UNIFORM_VEC4);

    Light sun = CreateLight(LIGHT_DIRECTIONAL, {10, 20, 5}, {0, 0, 0}, WHITE, shader);


    while (!windowManager.ShouldClose()) {
      // Lock the mouse only after clicking the window, so it can't grab it while hidden behind other apps
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsCursorHidden()) DisableCursor();
      if (!IsWindowFocused() && IsCursorHidden()) EnableCursor();

      // camera.Update(CAMERA_ORBITAL);

      if (raylib::Keyboard::IsKeyPressed(KEY_ESCAPE)) EnableCursor();



      windowManager.BeginFrame();

      float camPos[3] = {camera.position.x, camera.position.y, camera.position.z};
      shader.SetValue(shader.locs[SHADER_LOC_VECTOR_VIEW], camPos, SHADER_UNIFORM_VEC3);
      UpdateLightValues(shader, sun);

      camera.BeginMode();
      shader.BeginMode();


      for (raylib::Vector2 pos : floor.Get()) {
        DrawCube({pos.x, 0.5f, pos.y}, 1, 1, 1, RED);
      }
      for (raylib::Vector2 *pos : map.Get()) {
        if (pos == nullptr) continue;
        DrawCube({pos->x, 1.5f, pos->y}, 1, 1, 1, BLUE);
      }

      shader.EndMode();
      DrawGrid(10, 1.0f);


      camera.EndMode();
      windowManager.EndFrame();
    }

    for (raylib::Vector2 *pos : map.Get())
      delete pos;


  } catch (const std::exception &e) {
    printf("BOSError: %s\n", e.what());
  }

  delete reader;

  printf("\nPress Enter to exit...");
}
