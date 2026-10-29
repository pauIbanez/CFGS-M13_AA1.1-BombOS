#include "Player.h"
#include <raylib-cpp/Keyboard.hpp>
#include <raylib-cpp/Vector2.hpp>

namespace BombOS {

void Player::Update() {
  raylib::Vector2 dir;
  if (raylib::Keyboard::IsKeyDown(KEY_W)) dir.x += 1;
  if (raylib::Keyboard::IsKeyDown(KEY_A)) dir.y -= 1;
  if (raylib::Keyboard::IsKeyDown(KEY_S)) dir.x -= 1;
  if (raylib::Keyboard::IsKeyDown(KEY_D)) dir.y += 1;

  dir = dir.Normalize();
  position += raylib::Vector3{dir.x, position.y, dir.y} * 10 * GetFrameTime();
}

void Player::Render() { DrawCubeV(position, {0.2f, 1, 0.2f}, YELLOW); }
} // namespace BombOS
