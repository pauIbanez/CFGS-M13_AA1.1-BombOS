#!/usr/bin/env bash
set -euo pipefail

source "$(dirname "$0")/env.sh"

#Nuclear clean
rm -rf ./out

# Configure
cmake --preset release

# Build
cmake --build --preset release

# Install into out/<APP_NAME> (folder name comes from CMakeLists.txt)
cmake --install build/release --prefix out

# Zip it as out/<APP_NAME>-<version>-<platform>.zip
cpack --config build/release/CPackConfig.cmake -B build/release/package
cp build/release/package/*.zip out/
