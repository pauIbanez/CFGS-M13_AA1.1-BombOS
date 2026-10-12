#include "reader.h"
#include <cstdio>
#include <exception>


using namespace BombOS;

int main() {
  Reader *readerBOS = new Reader("./assets/level1.bos");
  Reader *readerPBOS = new Reader("./assets/level1_p.bos");

  printf("Reading BOS:\n");
  try {
    readerBOS->Read();
  } catch (std::exception e) {
    printf("BOSError: %s\n", e.what());
  }

  printf("\n\nReading PBOS:\n");
  try {
    readerPBOS->Read();
  } catch (std::exception e) {
    printf("PBOSError: %s\n", e.what());
  }

  delete readerBOS;
  delete readerPBOS;
}
