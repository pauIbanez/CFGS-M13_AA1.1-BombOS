#pragma once
#include "raylib-cpp.hpp"
#include <raylib-cpp/Vector3.hpp>


namespace BombOS {
class Player {

public:
  static const raylib::Vector3 size;

public:
  const short id;
  raylib::Vector3 position{0, 1, 0};

public:
  Player(const short &id, const raylib::Vector3 &position) : id(id), position(position) {}


public:
  void Update();
  void Render(const raylib::Vector3 &offset = {});
};
} // namespace BombOS
