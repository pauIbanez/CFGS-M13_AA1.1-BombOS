#include "Player.h"
#include <raylib-cpp/Keyboard.hpp>

namespace BombOS {

const float Player::speed = 3;

void Player::Update() {
  raylib::Vector2 dir;
  if (raylib::Keyboard::IsKeyDown(KEY_W)) dir.y -= 1;
  if (raylib::Keyboard::IsKeyDown(KEY_A)) dir.x -= 1;
  if (raylib::Keyboard::IsKeyDown(KEY_S)) dir.y += 1;
  if (raylib::Keyboard::IsKeyDown(KEY_D)) dir.x += 1;

  dir = dir.Normalize();
  position += dir * speed * GetFrameTime();

  if (dir.Length() > 0) {
    facing = dir;
    SetAnimation(Animation::Run);
  } else {
    SetAnimation(Animation::Idle);
  }

  animationTime += GetFrameTime();
}

void Player::SetAnimation(Animation next) {
  if (animation == next) return;
  animation = next;
  animationTime = 0;
}
} // namespace BombOS
