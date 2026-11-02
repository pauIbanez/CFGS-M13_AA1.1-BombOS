#include "Player.h"
#include "Level.h"
#include <algorithm>
#include <cmath>
#include <raylib-cpp/Keyboard.hpp>

namespace BombOS {

const float Player::speed = 3;
const float Player::cornerTolerance = 0.3f;
const float Player::epsilon = 0.01f;

namespace {
struct DirectionKey {
  KeyboardKey key;
  raylib::Vector2 direction;
};

const DirectionKey directionKeys[] = {
    {KEY_W, {0, -1}},
    {KEY_A, {-1, 0}},
    {KEY_S, {0, 1}},
    {KEY_D, {1, 0}},
};
} // namespace

void Player::Update() {
  const raylib::Vector2 dir = GetInputDir();
  const raylib::Vector2 previous = position;

  Move(dir * speed * GetFrameTime());


  if (dir.x != 0 || dir.y != 0) facing = dir;
  SetAnimation(position == previous ? Animation::Idle : Animation::Run);
  animationTime += GetFrameTime();
}


raylib::Vector2 Player::GetInputDir() {
  for (const auto &[key, direction] : directionKeys)
    if (raylib::Keyboard::IsKeyPressed(key)) lastKey = key;

  if (!raylib::Keyboard::IsKeyDown(lastKey)) {
    lastKey = KEY_NULL;
    for (const auto &[key, direction] : directionKeys) {
      if (!raylib::Keyboard::IsKeyDown(key)) continue;
      lastKey = key;
      break;
    }
  }

  for (const auto &[key, direction] : directionKeys)
    if (key == lastKey) return direction;
  return {0, 0};
}

void Player::SetAnimation(Animation next) {
  if (animation == next) return;
  animation = next;
  animationTime = 0;
}


void Player::Move(const raylib::Vector2 &delta) {
  const bool horizontal = delta.x != 0;
  const float signedStep = horizontal ? delta.x : delta.y;
  if (signedStep == 0) return;

  const float dir = signedStep > 0 ? 1 : -1;
  float step = std::abs(signedStep);

  const float main = horizontal ? position.x : position.y;
  float &cross = horizontal ? position.y : position.x;
  auto solidAt = [horizontal](float mainPos, float crossPos) {
    return Level::IsSolidAt(horizontal ? raylib::Vector2{mainPos, crossPos}
                                       : raylib::Vector2{crossPos, mainPos});
  };


  const float lane = std::floor(cross);
  const float offset = cross - lane;
  if (offset > epsilon && offset < 1 - epsilon) {
    const float ahead = dir > 0 ? std::floor(main) + 1 : std::ceil(main) - 1;
    const float nearest = offset < 0.5f ? lane : lane + 1;
    const float other = offset < 0.5f ? lane + 1 : lane;

    bool push = true;
    float target = nearest;
    if (solidAt(ahead, nearest)) {
      target = other;
      push = !solidAt(ahead, other) && std::abs(other - cross) <= cornerTolerance;
    }

    if (push) {
      const float shift = std::min(step, std::abs(target - cross));
      cross += target > cross ? shift : -shift;
      step -= shift;
      if (std::abs(target - cross) < epsilon) cross = target;
    }
  }

  if (step <= 0) return;
  if (horizontal) MoveX(dir * step);
  else MoveY(dir * step);
}

void Player::MoveX(const float &deltaX) {
  position.x += deltaX;
  const float top = position.y;
  const float bottom = position.y + 1 - epsilon;

  if (deltaX > 0) {
    const float right = position.x + 1 - epsilon;
    if (Level::IsSolidAt({right, top}) || Level::IsSolidAt({right, bottom}))
      position.x = std::floor(right) - 1;
  } else if (deltaX < 0) {
    if (Level::IsSolidAt({position.x, top}) || Level::IsSolidAt({position.x, bottom}))
      position.x = std::floor(position.x) + 1;
  }
}

void Player::MoveY(const float &deltaY) {
  position.y += deltaY;
  const float left = position.x;
  const float right = position.x + 1 - epsilon;

  if (deltaY > 0) {
    const float bottom = position.y + 1 - epsilon;
    if (Level::IsSolidAt({left, bottom}) || Level::IsSolidAt({right, bottom}))
      position.y = std::floor(bottom) - 1;
  } else if (deltaY < 0) {
    if (Level::IsSolidAt({left, position.y}) || Level::IsSolidAt({right, position.y}))
      position.y = std::floor(position.y) + 1;
  }
}
} // namespace BombOS
