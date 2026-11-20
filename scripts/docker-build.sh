#!/usr/bin/env bash
# Builds a CMake project that uses the emulator inside the emulator's container.
#   scripts/docker-build.sh [PROJECT_DIR] [BUILD_DIR]     (defaults: this repo, build/)
set -euo pipefail
here="$(cd "$(dirname "$0")/.." && pwd)"
project="$(cd "${1:-$here}" && pwd)"
build="${2:-build}"
docker build -q -t esphome-lvgl-emulator "$here" >/dev/null
docker run --rm -u "$(id -u):$(id -g)" -e HOME=/tmp -v "$project":/src -w /src esphome-lvgl-emulator \
  sh -c "cmake -S . -B '$build' -DCMAKE_BUILD_TYPE=Release >/dev/null && cmake --build '$build' -j\"\$(nproc)\""
