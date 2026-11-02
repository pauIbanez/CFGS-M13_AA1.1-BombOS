#pragma once

#include "WindowManager.h"
#include <memory>
#include <raylib-cpp.hpp>
#include <string>

namespace BombOS {
class GameManager {
public:
  static bool isPaused;

  static inline WindowManager &GetWindow() { return *window; }
  static inline raylib::Camera3D &GetCamera() { return camera; }

private:
  static bool isHalted;

  static std::unique_ptr<WindowManager> window;
  static raylib::Camera3D camera;

public:
  static void Init();
  static void Update();
  static void Render();
  static void Pause();
  static void Resume();
  static void LoadLevel(const std::string &levelName);
  static void Shutdown();

  static bool ShouldClose();
};
} // namespace BombOS
