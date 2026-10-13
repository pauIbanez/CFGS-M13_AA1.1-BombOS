

#include "PBOSReader.h"
namespace BombOS {

const std::unique_ptr<Map::MapData> PBOSReader::Read() {
  const std::string title = ReadTitle();
  const Map::TextureStore textureStore = ReadTextures();

  return nullptr;
}


const Map::TextureStore PBOSReader::ReadTextures() {
  ExpectHeader("TEXTURES");

  Map::TextureStore textureStore;
  std::string texture = GetNext();
  short counter = 0;
  while (!IsLineEnd(texture)) {
    textureStore.insert({std::to_string(counter), texture});
    counter++;
    texture = GetNext();
  }

  for (auto [k, v] : textureStore) {
    printf("%s: %s\n", k.c_str(), v.c_str());
  }

  return textureStore;
}


void PBOSReader::ReadTexturesPBOS(Map::TextureStore &textures) {}

} // namespace BombOS
