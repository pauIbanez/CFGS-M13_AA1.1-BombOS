#!/usr/bin/env bash
set -euo pipefail

printf "Starting watcher...\n"

watchexec -w src -e cpp,h -r --stop-signal SIGTERM -- bash -lc '
  clear
  bash ./scripts/build.sh
'
