#include "Reader.h"
#include "WindowManager.h"
#include <cstdio>
#include <exception>


using namespace BombOS;

int main() {
  Reader *reader = new Reader("./assets/level1.bos");

  try {
    reader->Read();
    WindowManager::Init();
  } catch (const std::exception &e) {
    printf("BOSError: %s\n", e.what());
  }



  delete reader;
  printf("\nPress Enter to exit...");
}
