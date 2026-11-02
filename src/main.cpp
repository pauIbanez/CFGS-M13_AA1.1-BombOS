#include "GameManager.h"
#include <cstdio>
#include <exception>



using namespace BombOS;



int main() {
  try {
    GameManager::Init();

    while (!GameManager::ShouldClose()) {
      GameManager::Update();
      GameManager::Render();
    }

  } catch (const std::exception &e) {
    printf("Error: %s\n", e.what());
  }

  GameManager::Shutdown();

  printf("\nPress Enter to exit...");
}
