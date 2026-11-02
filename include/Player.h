#pragma once
#include "raylib-cpp.hpp"
#include <raylib-cpp/Vector2.hpp>


namespace BombOS {
class Player {

public:
  enum class Animation { Idle, Run, Victory, Count };

  static const float speed;

public:
  const short id;
  raylib::Vector2 position;

  // Unit direction in grid space the player is looking at
  raylib::Vector2 facing{1, 0};
  Animation animation = Animation::Idle;
  float animationTime = 0;

public:
  Player(const short &id, const raylib::Vector2 &position) : id(id), position(position) {}


public:
  void Update();
  void SetAnimation(Animation next);
};
} // namespace BombOS
