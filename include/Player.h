#pragma once
#include "raylib-cpp.hpp"


namespace BombOS {
class Player {
public:
  raylib::Vector3 position{0, 1, 0};



public:
  void Update();
  void Render();
};
} // namespace BombOS
