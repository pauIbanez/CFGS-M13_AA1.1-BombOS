#pragma once
#include "raylib-cpp.hpp"
#include <raylib-cpp/Vector2.hpp>


namespace BombOS {
class Player {

public:
  static const float speed;

public:
  const short id;
  raylib::Vector2 position;

public:
  Player(const short &id, const raylib::Vector2 &position) : id(id), position(position) {}


public:
  void Update();
};
} // namespace BombOS
