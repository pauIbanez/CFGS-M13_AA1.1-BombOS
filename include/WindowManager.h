

#pragma once


namespace BombOS {
class WindowManager {
public:
  static const int screenWidth = 800;
  static const int screenHeight = 450;

  static void Init();
  static void Shutdown();
  static bool ShouldClose();
  static void BeginFrame();
  static void EndFrame();
};
} // namespace BombOS
