#pragma once
#include "raylib-cpp.hpp"
#include <raylib-cpp/Vector2.hpp>


namespace BombOS {
class Player {

public:
  enum class Animation { Idle, Run, Victory, Count };
  static const float speed;
  static const float cornerTolerance;
  static const float epsilon;

public:
  const short id;
  raylib::Vector2 position;

  raylib::Vector2 facing{1, 0};
  Animation animation = Animation::Idle;
  float animationTime = 0;

private:
  KeyboardKey lastKey = KEY_NULL;

public:
  Player(const short &id, const raylib::Vector2 &position) : id(id), position(position) {}


public:
  void Update();
  void SetAnimation(Animation next);

private:
  raylib::Vector2 GetInputDir();
  void Move(const raylib::Vector2 &delta);
  void MoveX(const float &deltaX);
  void MoveY(const float &deltaY);
};
} // namespace BombOS
