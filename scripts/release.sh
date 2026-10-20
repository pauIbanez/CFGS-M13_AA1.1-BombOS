#!/usr/bin/env bash
set -euo pipefail

#Nuclear clean
rm -rf ./out

# Configure
cmake --preset release

# Build
cmake --build --preset release

# Install into out/<APP_NAME> (folder name comes from CMakeLists.txt)
cmake --install build/release --prefix out
