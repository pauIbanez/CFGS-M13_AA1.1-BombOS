

#include "BOSReader.h"
#include <cstdio>
namespace BombOS {

const std::unique_ptr<Map::MapData> BOSReader::Read() {
  const std::string title = ReadTitle();
  const Map::TextureStore textureStore = ReadTextures();
  const Map::MapSize mapSize = ReadSize();
  const Map::StringMap floor = ReadFloor(textureStore, mapSize);
  const Map::StringMap main = ReadMap(textureStore, mapSize);
  const Map::StringMap collisions = ReadCollisions(mapSize);

  return std::make_unique<Map::MapData>(title, textureStore, mapSize, floor, main, collisions);
}


const Map::TextureStore BOSReader::ReadTextures() {
  ExpectHeader("TEXTURES");

  Map::TextureStore textureStore;
  std::string sizeStr = GetNext();
  int size;
  try {
    size = std::stoi(sizeStr);
  } catch (const std::exception &) {
    throw std::exception("Invalid texture size");
  }

  if (size < 0) throw std::exception("Invalid texture size");

  printf("Reading %i textures\n", size);

  ExpectLineEnd("Texture size");
  for (size_t i = 0; i < size; i++) {
    std::string ident = GetNext();
    std::string textureName = GetNext();
    ExpectLineEnd(textureName);
    if (!textureStore.emplace(ident, textureName).second)
      throw std::exception(("Duplicate texture identifier: " + ident).c_str());
  }

  for (auto [k, v] : textureStore) {
    printf("%s: %s\n", k.c_str(), v.c_str());
  }

  return textureStore;
}

const Map::MapSize BOSReader::ReadSize() {
  ExpectHeader("SIZE");
  std::string widthStr = GetNext();
  std::string heightStr = GetNext();

  short width;
  short height;
  try {
    width = std::stoi(widthStr);
    height = std::stoi(heightStr);
  } catch (const std::exception &) {
    throw std::exception("Invalid size");
  }

  if (height <= 0 || width <= 0) {
    throw std::exception("Size must be higher than 0!");
  }

  ExpectLineEnd("Size");


  printf("SIZE: %i %i\n", width, height);
  return Map::MapSize(width, height);
}

const Map::StringMap BOSReader::ReadFloor(const Map::TextureStore &store,
                                          const Map::MapSize &size) {
  ExpectHeader("FLOOR");
  ExpectLineEnd("Floor header");

  Map::StringMap floorMap(size.width, size.height);

  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      std::string textureIdent = GetNext();
      EnsureRegistered(store, textureIdent);
      floorMap.At(x, y) = textureIdent;
    }
    ExpectLineEnd("Floor column: " + std::to_string(y));
  }

  short i = 0;
  for (std::string tile : floorMap.Get()) {
    printf("%s", tile.c_str());
    i++;
    if (i % size.width == 0) printf("\n");
  }



  return floorMap;
}

const Map::StringMap BOSReader::ReadMap(const Map::TextureStore &store, const Map::MapSize &size) {
  ExpectHeader("MAP");
  ExpectLineEnd("Map header");

  Map::StringMap mainMap(size.width, size.height);

  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      std::string textureIdent = GetNext();
      EnsureRegistered(store, textureIdent);
      mainMap.At(x, y) = textureIdent;
    }
    ExpectLineEnd("Map column: " + std::to_string(y));
  }


  return mainMap;
}

const Map::StringMap BOSReader::ReadCollisions(const Map::MapSize &size) {
  ExpectHeader("COLLISIONS");
  ExpectLineEnd("Collisions header");

  Map::StringMap collisionMap(size.width, size.height);

  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      std::string collisionType = GetNext();
      // TODO: Check accepted collision types. Throw or grab default.
      // TODO: Consider creating a CollisionMap and parse the string to a CollisionType enum;
      collisionMap.At(x, y) = collisionType;
    }
    ExpectLineEnd("Column: " + std::to_string(y));
  }

  return collisionMap;
}



const bool BOSReader::EnsureRegistered(const Map::TextureStore &store,
                                       const std::string &textureIdent) const {

  if (textureIdent == "0") return true;
  auto it = store.find(textureIdent);
  if (it == store.end())
    throw std::exception(
        ("Texture identifyer " + textureIdent + " was not found in the store").c_str());
  return true;
}

} // namespace BombOS
