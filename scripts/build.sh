#!/usr/bin/env bash
set -euo pipefail

printf "Parsing config..\n"

get_config() {
  config="$1"
  input="$2"
  if [[ $input =~ $config[[:space:]]*=[[:space:]]*(.*) ]]; then
    value="${BASH_REMATCH[1]}"
    echo "$value"
    return 0
  fi
  return 1
}

appName=""
buildDir=""
defaultBuildDir="build"

while IFS= read -r line; do
  if appName=$(get_config "AppName" "$line"); then
    break
  fi
  if buildDir=$(get_config "BuildDir" "$line"); then
    continue
  fi
done <.config

if [[ -z $appName ]]; then
  echo "Error: AppName not found in .config" >&2
  exit 1
fi

if [[ -z ${buildDir} ]]; then
  echo "Warning: BuildDir not found in .config, using default dir $defaultBuildDir" >&2
  buildDir=$defaultBuildDir
fi

printf "Checking folders...\n"
mkdir -p ${buildDir} ${buildDir}/obj

printf "Copying assets...\n"
rm -rf ./${buildDir}/assets
cp -r ./assets ./${buildDir}

printf "Building...\n"

CXX="${CXX:-g++}"
CXXFLAGS="-std=c++20 -Wall -O2"
PKG_CFLAGS="$(pkg-config --cflags sdl3 sdl3-image)"
PKG_LIBS="$(pkg-config --libs sdl3 sdl3-image)"

OBJDIR="./${buildDir}/obj"
tmp="./${buildDir}/${appName}.$$.new"

# Return 0 (true) if object needs rebuild
needs_rebuild() {
  local src="$1" obj="$2" dep="$3"

  # Missing output or depfile -> rebuild
  [[ ! -f "$obj" || ! -f "$dep" ]] && return 0

  # Source newer than object -> rebuild
  [[ "$src" -nt "$obj" ]] && return 0

  # Parse depfile; rebuild if any listed dependency is newer than obj or missing.
  # Assumes no spaces in file paths (typical for C/C++ projects).
  local joined deps f
  joined="$(sed -e ':a' -e 'N' -e '$!ba' -e 's/\\\n/ /g' "$dep")"
  deps="$(printf '%s' "$joined" | sed -e 's/^[^:]*:[[:space:]]*//')"

  for f in $deps; do
    [[ ! -e "$f" ]] && return 0
    [[ "$f" -nt "$obj" ]] && return 0
  done

  return 1
}

# Collect sources
mapfile -t SRCS < <(find src -type f -name "*.cpp" | sort)

# Compile changed units
changed=0
OBJS=()

for s in "${SRCS[@]}"; do
  o="$OBJDIR/$s.o"
  d="$o.d"
  mkdir -p "$(dirname "$o")"
  OBJS+=("$o")

  if needs_rebuild "$s" "$o" "$d"; then
    ccache "$CXX" $CXXFLAGS $PKG_CFLAGS -MMD -MP -c "$s" -o "$o"
    changed=1
  fi
done

# Optional: clean stale objects for deleted sources (keeps objdir tidy)
# This is safe and helps avoid linking unused objects if you ever glob them.
while IFS= read -r -d '' obj; do
  # obj path is buildDir/obj/src/.../file.cpp.o -> source is src/.../file.cpp
  src="${obj#"$OBJDIR/"}"
  src="${src%.o}"
  if [[ ! -f "$src" ]]; then
    rm -f "$obj" "$obj.d"
    changed=1
  fi
done < <(find "$OBJDIR/src" -type f -name "*.cpp.o" -print0 2>/dev/null || true)

# Link (only if something changed OR app missing)
if [[ $changed -eq 1 || ! -f "./${buildDir}/${appName}" ]]; then
  ccache "$CXX" "${OBJS[@]}" -o "$tmp" $PKG_LIBS
  mv -f "$tmp" "./${buildDir}/${appName}"
else
  echo "Up to date."
fi

printf "Done: ./${buildDir}/${appName}\n"
