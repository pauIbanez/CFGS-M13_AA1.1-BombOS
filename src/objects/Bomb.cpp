#include "Bomb.h"
#include "Level.h"
#include "Player.h"
#include <memory>
#include <raylib-cpp/Vector2.hpp>
#include <raylib.h>

namespace BombOS {

const float Bomb::DelayTime = 3;
const short Bomb::Range = 2;

void Bomb::Update() {
  CheckSolid();
  RunTimer();
}

void Bomb::CheckSolid() {
  if (isSolid) return;

  const float bombLeft = position.x;
  const float bombRight = position.x + 1;
  const float bombTop = position.y;
  const float bombBottom = position.y + 1;

  for (const std::unique_ptr<Player> &player : Level::GetPlayers()) {
    if (!player) continue;

    const float playerLeft = player->position.x;
    const float playerRight = player->position.x + 1;
    const float playerTop = player->position.y;
    const float playerBottom = player->position.y + 1;

    const bool overlaps =
        playerRight > bombLeft + Player::epsilon && playerLeft < bombRight - Player::epsilon &&
        playerBottom > bombTop + Player::epsilon && playerTop < bombBottom - Player::epsilon;

    if (overlaps) return;
  }
  isSolid = true;
}

void Bomb::RunTimer() {
  if (hasDetonated || timer < 0) return;

  timer -= GetFrameTime();
  if (timer <= 0) {
    Detonate();
    hasDetonated = true;
  }
}

bool Bomb::IsSolid() const { return isSolid; }


void Bomb::Detonate() {
  if (hasDetonated) return;

  const raylib::Vector2 &p = position;
  const raylib::Vector2 directions[] = {
      raylib::Vector2{0, 1},
      raylib::Vector2{1, 0},
      raylib::Vector2{0, -1},
      raylib::Vector2{-1, 0},
  };

  for (const raylib::Vector2 &dir : directions) {
    for (short i = 1; i <= Range; i++) {
      const raylib::Vector2 tile = position + dir * i;
      if (Level::DestroyBlock(tile)) break;
    }
  }

  hasDetonated = true;
}
} // namespace BombOS
