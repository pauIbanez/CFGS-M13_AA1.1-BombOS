#include "Player.h"
#include <raylib-cpp/Keyboard.hpp>

namespace BombOS {

const float Player::speed = 10;

void Player::Update() {
  raylib::Vector2 dir;
  if (raylib::Keyboard::IsKeyDown(KEY_W)) dir.y -= 1;
  if (raylib::Keyboard::IsKeyDown(KEY_A)) dir.x -= 1;
  if (raylib::Keyboard::IsKeyDown(KEY_S)) dir.y += 1;
  if (raylib::Keyboard::IsKeyDown(KEY_D)) dir.x += 1;

  dir = dir.Normalize();
  position += dir * speed * GetFrameTime();
}
} // namespace BombOS
