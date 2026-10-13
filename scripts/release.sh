#Nuclear clean
rm -rf ./out

# Configure
cmake --preset release

# Build
cmake --build --preset release

# Install both into the same SDK prefix
cmake --install build/release --prefix out/BombOS
