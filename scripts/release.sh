# Nuclear clean
rm -rf build/sdk-debug build/sdk-release out/Crow2D_SDK

# Configure
cmake --preset sdk-debug
cmake --preset sdk-release

# Build
cmake --build --preset sdk-debug
cmake --build --preset sdk-release

# Install both into the same SDK prefix
cmake --install build/sdk-debug --prefix out/Crow2D_SDK
cmake --install build/sdk-release --prefix out/Crow2D_SDK
