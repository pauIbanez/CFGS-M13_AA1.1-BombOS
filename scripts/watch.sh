#!/usr/bin/env bash
set -euo pipefail

printf "Starting watcher...\n"

run_after=1
while getopts "n" opt; do
  case "$opt" in
  n) run_after=0 ;;
  esac
done
shift $((OPTIND - 1))

# Watch code, build config and data folders (only the ones that exist)
watch_args=()
for path in src include cmake assets resources CMakeLists.txt CMakePresets.json; do
  [[ -e "$path" ]] && watch_args+=(-w "$path")
done

watchexec "${watch_args[@]}" -r \
  -E RUN_AFTER="$run_after" \
  -- bash ./scripts/dev.sh
