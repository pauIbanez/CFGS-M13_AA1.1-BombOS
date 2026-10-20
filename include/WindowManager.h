#pragma once
#include <raylib-cpp.hpp>


namespace BombOS {
class WindowManager {
public:
  static constexpr int screenWidth = 800;
  static constexpr int screenHeight = 450;

  WindowManager();

  bool ShouldClose() const;
  void BeginFrame();
  void EndFrame();

private:
  raylib::Window window;
};
} // namespace BombOS
