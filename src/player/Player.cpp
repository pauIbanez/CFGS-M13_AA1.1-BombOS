#include "Player.h"
#include <raylib-cpp/Keyboard.hpp>
#include <raylib-cpp/Vector2.hpp>

namespace BombOS {

const raylib::Vector3 Player::size{0.8, 2, 0.8};

void Player::Update() {
  raylib::Vector2 dir;
  if (raylib::Keyboard::IsKeyDown(KEY_W)) dir.x += 1;
  if (raylib::Keyboard::IsKeyDown(KEY_A)) dir.y -= 1;
  if (raylib::Keyboard::IsKeyDown(KEY_S)) dir.x -= 1;
  if (raylib::Keyboard::IsKeyDown(KEY_D)) dir.y += 1;

  dir = dir.Normalize();
  position += raylib::Vector3{dir.x, 0, dir.y} * 10 * GetFrameTime();
}

void Player::Render(const raylib::Vector3 &offset) { DrawCubeV(position + offset, size, YELLOW); }
} // namespace BombOS
