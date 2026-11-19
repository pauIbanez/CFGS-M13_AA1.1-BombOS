#include "Bomb.h"
#include "Level.h"
#include <cstddef>
#include <raylib-cpp/Vector2.hpp>
#include <raylib.h>

namespace BombOS {

const float Bomb::DelayTime = 1000;
const short Bomb::Range = 2;

void Bomb::Update() {
  if (hasDetonated || timer < 0) return;

  timer -= GetFrameTime();
  if (timer <= 0) {
    hasDetonated = true;
    Detonate();
  }
}


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
}
} // namespace BombOS
