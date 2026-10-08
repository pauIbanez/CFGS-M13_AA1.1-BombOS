#include "reader.h"
#include <cstdio>


using namespace BombOS;

int main() {

  Reader *reader = new Reader("./assets/t.map");
  reader->Read();
  delete reader;
}
