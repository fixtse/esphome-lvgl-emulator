# Build and run environment: compilers, CMake, FreeType, SDL2, rsvg-convert.
FROM debian:trixie-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential cmake git ca-certificates pkg-config libfreetype-dev libsdl2-dev librsvg2-bin fonts-dejavu-core \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
