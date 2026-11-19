#pragma once

#include "raylib-cpp.hpp"


namespace BombOS {

class Bomb {
public:
  const static float DelayTime;
  const static short Range;

public:
  raylib::Vector2 position;
  bool hasDetonated = false;
  float timer;

private:
  bool isSolid = false;

public:
  Bomb(raylib::Vector2 position) : position(position) { timer = DelayTime; }

public:
  void Update();
  void Detonate();
  bool IsSolid() const;

private:
  void CheckSolid();
  void RunTimer();
};
} // namespace BombOS
