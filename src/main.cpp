#include "reader.h"
#include <cstdio>
#include <exception>


using namespace BombOS;

int main() {
  Reader *reader = new Reader("./assets/b.map");
  try {
    reader->Read();
  } catch (std::exception e) {
    printf("Error: %s\n", e.what());
  }

  delete reader;
}
