#!/usr/bin/env bash
# Runs a built emulator app in the container: scripts/docker-run.sh PROJECT_DIR build/my-app [ARGS...]
set -euo pipefail
here="$(cd "$(dirname "$0")/.." && pwd)"
project="$(cd "$1" && pwd)"; shift
docker build -q -t esphome-lvgl-emulator "$here" >/dev/null
docker run --rm -u "$(id -u):$(id -g)" -e HOME=/tmp -v "$project":/src -w /src esphome-lvgl-emulator "$@"
