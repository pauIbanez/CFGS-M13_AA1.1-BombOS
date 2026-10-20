

#pragma once
#include <raylib-cpp.hpp>


namespace BombOS {
class WindowManager {
public:
  static constexpr int screenWidth = 800;
  static constexpr int screenHeight = 450;

  static void Init();
  static bool ShouldClose() { return window.ShouldClose(); }
  static void BeginFrame();
  static void EndFrame();

private:
  static raylib::Window window;
};
} // namespace BombOS
