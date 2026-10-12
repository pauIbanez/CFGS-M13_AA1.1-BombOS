

#include "Reader.h"
#include <charconv>
namespace BombOS {

const char Reader::delimeter = ';';
const std::unordered_map<std::string, Format> Reader::formatHeaders = {
    {"BOS", Format::BOS},
    {"PBOS", Format::PBOS},
};

const Version Reader::PBOSlatest = Version(0, 0, 1);
const Version Reader::BOSlatest = Version(0, 0, 5);



Reader::~Reader() {
  if (stream && stream.is_open()) stream.close();
  delete version;
}

const std::unique_ptr<Map::MapData> Reader::Read() {
  stream = std::ifstream(filePath);
  if (!stream) {
    throw std::exception(("Could not open file: " + filePath).c_str());
  }


  ReadFormat();
  ReadVersion();

  const std::string title = ReadTitle();
  const Map::TextureStore textureStore = ReadTextures();
  const Map::MapSize mapSize = ReadSize();
  const Map::FlatMap floor = ReadFloor(textureStore, mapSize);
  const Map::FlatMap main = ReadMap(textureStore, mapSize);
  const Map::FlatMap collisions = ReadCollisions(mapSize);

  return std::make_unique<Map::MapData>(title, textureStore, mapSize, floor, main, collisions);
}


void Reader::ReadFormat() {
  std::string formatStr = GetNext();

  if (!TryParseFormat(formatStr, format)) {
    throw std::exception(("Invalid format found " + formatStr).c_str());
  }

  printf("Reading format %s\n", formatStr.c_str());
}

void Reader::ReadVersion() {
  std::string versionStr = GetNext();

  if (!TryParseVersion(versionStr, version)) {
    throw std::exception(("Invalid version found " + versionStr).c_str());
  }


  switch (format) {
  case Format::BOS:
    if (*version > BOSlatest) {
      throw std::exception(("Version " + version->ToString() +
                            " is unsupported! Highest BOS version is " + BOSlatest.ToString())
                               .c_str());
    }
    break;

  case Format::PBOS:
    if (*version > PBOSlatest) {
      throw std::exception(("Version " + version->ToString() +
                            " is unsupported! Highest PBOS version is " + PBOSlatest.ToString())
                               .c_str());
    }
    break;
  }
  ExpectLineEnd("Version");

  printf("Reading version %s\n", version->ToString().c_str());
}

const std::string Reader::ReadTitle() {

  ExpectHeader("TITLE");

  std::string title = GetNext();
  if (title == "") throw std::exception("Can't have empty title");

  ExpectLineEnd("Title");

  printf("TITLE: %s\n", title.c_str());
  return title;
}

const Map::TextureStore Reader::ReadTextures() {
  ExpectHeader("TEXTURES");

  Map::TextureStore textureStore;
  switch (format) {
  case Format::BOS:
    ReadTexturesBOS(textureStore);
    break;

  case Format::PBOS:
    ReadTexturesPBOS(textureStore);
    break;
  }

  for (auto [k, v] : textureStore) {
    printf("%s: %s\n", k.c_str(), v.c_str());
  }

  return textureStore;
}

const Map::MapSize Reader::ReadSize() {
  ExpectHeader("SIZE");
  std::string heightStr = GetNext();
  std::string widthStr = GetNext();

  short height;
  short width;
  try {
    height = std::stoi(heightStr);
    width = std::stoi(widthStr);
  } catch (const std::exception &) {
    throw std::exception("Invalid size");
  }

  if (height <= 0 || width <= 0) {
    throw std::exception("Size must be higher than 0!");
  }

  ExpectLineEnd("Size");


  printf("SIZE: %i %i\n", height, width);
  return Map::MapSize(height, width);
}

const Map::FlatMap Reader::ReadFloor(const Map::TextureStore &store, const Map::MapSize &size) {
  ExpectHeader("FLOOR");
  ExpectLineEnd("Floor header");

  Map::FlatMap floorMap(size.height, size.width);

  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      std::string textureIdent = GetNext();
      EnsureRegistered(store, textureIdent);
      floorMap.at(y, x) = textureIdent;
    }
    ExpectLineEnd("Floor column: " + std::to_string(y));
  }

  return floorMap;
}

const Map::FlatMap Reader::ReadMap(const Map::TextureStore &store, const Map::MapSize &size) {
  ExpectHeader("MAP");
  ExpectLineEnd("Map header");

  Map::FlatMap mainMap(size.height, size.width);

  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      std::string textureIdent = GetNext();
      EnsureRegistered(store, textureIdent);
      mainMap.at(y, x) = textureIdent;
    }
    ExpectLineEnd("Map column: " + std::to_string(y));
  }

  return mainMap;
}

const Map::FlatMap Reader::ReadCollisions(const Map::MapSize &size) {
  ExpectHeader("COLLISIONS");
  ExpectLineEnd("Collisions header");

  Map::FlatMap collisionMap(size.height, size.width);

  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      std::string collisionType = GetNext();
      // TODO: Check accepted collision types. Throw or grab default.
      // TODO: Consider creating a CollisionMap and parse the string to a CollisionType enum;
      collisionMap.at(y, x) = collisionType;
    }
    ExpectLineEnd("Column: " + std::to_string(y));
  }

  return collisionMap;
}


void Reader::ReadTexturesBOS(Map::TextureStore &textures) {
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
    if (!textures.emplace(ident, textureName).second)
      throw std::exception(("Duplicate texture identifier: " + ident).c_str());
  }
}

void Reader::ReadTexturesPBOS(Map::TextureStore &textures) {

  std::string texture = GetNext();
  short counter = 0;
  while (!IsLineEnd(texture)) {
    textures.insert({std::to_string(counter), texture});
    counter++;
    texture = GetNext();
  }
}


const bool Reader::TryParseFormat(std::string &formatStr, Format &format) {

  auto it = formatHeaders.find(formatStr);
  if (it == formatHeaders.end()) {
    return false;
  }
  format = it->second;
  return true;
}

const bool Reader::TryParseVersion(std::string &versionStr, Version *&version) {
  std::vector<std::string> parts;
  std::string part = "";
  const char splitter = '.';

  for (size_t i = 0; i < versionStr.length(); i++) {
    if (versionStr[i] == splitter) {
      parts.push_back(part);
      part = "";
      continue;
    }
    part += versionStr[i];
  }
  parts.push_back(part);

  if (format == Format::PBOS && parts.size() != 3) {
    throw std::exception(("Version must be major.minor.patch, found " + versionStr).c_str());
  }
  if (format == Format::BOS && parts.size() != 2) {
    throw std::exception(("Version must be major.patch, found " + versionStr).c_str());
  }

  short numbers[3] = {0, 0, 0};
  for (size_t i = 0; i < parts.size(); i++) {
    const char *begin = parts[i].data();
    const char *end = begin + parts[i].size();
    auto [ptr, ec] = std::from_chars(begin, end, numbers[i]);
    if (ec != std::errc() || ptr != end || numbers[i] < 0) {
      throw std::exception(("Invalid version part '" + parts[i] + "' in " + versionStr).c_str());
    }
  }

  delete version;

  switch (format) {
  case Format::BOS:
    version = new Version(numbers[0], 0, numbers[1]);
    break;

  case Format::PBOS:
    version = new Version(numbers[0], numbers[1], numbers[2]);
    break;
  }
  return true;
}


const bool Reader::EnsureRegistered(const Map::TextureStore &store,
                                    const std::string &textureIdent) const {

  if (textureIdent == "0") return true;
  auto it = store.find(textureIdent);
  if (it == store.end())
    throw std::exception(
        ("Texture identifyer " + textureIdent + " was not found in the store").c_str());
  return true;
}


const std::string Reader::GetNext(const char &del) {
  std::string next;
  std::getline(stream, next, del);
  if (stream.eof()) throw std::exception("Unexpected end of file");

  const char *ws = "\r\n";
  size_t start = next.find_first_not_of(ws);
  if (start == std::string::npos) return "";
  size_t end = next.find_last_not_of(ws);
  return next.substr(start, end - start + 1);
}

void Reader::ExpectHeader(const std::string &header) {
  std::string next = GetNext();
  if (next != header)
    throw std::exception(("Unexpected header. Got:" + next + " Expected: " + header).c_str());
}

const bool Reader::IsLineEnd(const std::string &read) const { return read == ""; }

void Reader::ExpectLineEnd(const std::string &after) {
  std::string next = GetNext();
  if (!IsLineEnd(next))
    throw std::exception(("Expected line end after " + after + ", got: " + next).c_str());
}

void Reader::ExpectEnd() {
  stream >> std::ws;
  if (stream.peek() != std::char_traits<char>::eof())
    throw std::exception("Expected end of file, found more data");
}

} // namespace BombOS
