# esphome-lvgl-emulator

Run the LVGL UI code of an ESP32/ESPHome project on a PC. Your screens are
drawn by the real LVGL library, built with the same `lv_conf.h` as your
firmware, and written to PNGs (headless, good for CI and for checking layout
changes without flashing) or shown in an SDL window.

It is meant for projects that build their UI in C++ (ESPHome `includes:`,
lambdas or custom components) and want to check it at several screen sizes
without a board.

## What you get

- **LVGL on the host**, any version tag (default `v9.6.0`, the version in
  ESPHome 2026.11), built with your `lv_conf.h`.
- **ESPHome and ESP-IDF stand-ins** (`stubs/`) so firmware headers compile
  unchanged:
  - ESPHome: `millis()`, `ESP_LOG*`, preferences, `ESPTime`, `select`, `globals` and `id()`, the API server, `json`, `image`.
  - ESP-IDF: `nvs` (in memory), `esp_heap_caps`, `esp_err`, `esp_system`, base64, websocket and mDNS.
  
  Time is emulated, so renders are reproducible.
- **Fonts and images:**
  - TrueType fonts through FreeType, like ESPHome's `font:` component.
  - SVG and PNG images through `rsvg-convert` and LVGL's PNG decoder, like `image:`.
- **Output**: one PNG per screen, or an SDL window where the mouse is the
  touchscreen.

## Quick start

```sh
scripts/docker-build.sh                       # builds LVGL, the runtime and the example
scripts/docker-run.sh . build/hello-panel --out out
scripts/docker-run.sh . build/hello-panel --width 320 --height 480 --out out
```

Without Docker you need CMake ≥ 3.20, a C++20 compiler, FreeType, SDL2
(optional) and `rsvg-convert`; then `cmake -S . -B build && cmake --build build`.

## Your own app

An app defines one function: the screen size, a setup that builds the UI, and
the screens to capture.

```cpp
#include "lvgl_emu/runtime.h"
#include "esphome.h"          // the stand-ins
#include "my_ui.h"            // your firmware's UI code

lvgl_emu::App lvgl_emu_app(const lvgl_emu::Args &args) {
  lvgl_emu::App app;
  app.width = 320; app.height = 480;
  app.setup = [] { my_ui::create(lvgl_emu::ttf("fonts/Roboto.ttf", 20)); };
  app.screens = [] {
    return std::vector<lvgl_emu::Screen>{
        {"home", [] { my_ui::show_home(); }},
        {"settings", [] { my_ui::show_settings(); }}};
  };
  return app;
}
```

```cmake
include(FetchContent)
FetchContent_Declare(esphome_lvgl_emulator
  GIT_REPOSITORY https://github.com/fixtse/esphome-lvgl-emulator.git GIT_TAG main)
set(LVGL_EMU_LV_CONF ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE FILEPATH "" FORCE)   # optional
FetchContent_MakeAvailable(esphome_lvgl_emulator)
add_lvgl_emulator_app(my-ui SOURCES app.cpp INCLUDE_DIRS my_stubs path/to/firmware)
```

`INCLUDE_DIRS` come before the built-in stand-ins, so a project can add or
override a header (for example its own components, or the ESPHome globals its
code reads with `id()`).

Command line, handled by the runtime: `--out DIR`, `--sdl`, `--interval MS`
(SDL, per screen), `--only TEXT` (screens whose name contains TEXT). Any other
`--key value` reaches the app as `args.get("key")`.

Helpers for apps (`include/lvgl_emu/runtime.h`):
- `ttf(path, size)`
- `image(path, w, h)`: SVG, PNG or JPEG resized like ESPHome's `image:` `resize:`
- `svg(path, w, h)` (the same)
- `png(path)` (native size)
- `settle(ms)`
- `write_png(path)`
- `now_ms()`
- `substitutions(yaml)`: reads an ESPHome YAML's `substitutions:` so screen sizes and scales come from the same files as the firmware.

## Matching your firmware

- Copy `lv_conf.h` from your ESPHome build
  (`.esphome/build/<name>/src/lv_conf.h`) and add the host settings at the end
  of `config/lv_conf.h`:
  - FreeType
  - stdio file system `A:`
  - lodepng
  - `LV_USE_STDLIB_MALLOC LV_STDLIB_CLIB`
  - a built-in default font
  - the SDL block.
- Set `LVGL_EMU_LVGL_VERSION` to the LVGL version of your ESPHome release.
- Font rendering differs slightly from ESPHome's pre-rendered bitmaps;
  layout, sizes and spacing match.

## Limits

- The stand-ins compile the firmware APIs but do nothing: no Home Assistant
  connection, network, audio or hardware. Feed state into your UI from the
  app, for example from a scenario JSON file.
- ESPHome YAML `lvgl:` widget definitions are not translated; this runs C++ UI
  code. Widgets created in YAML must be recreated in the app if you need them.
- The stand-ins cover what real projects needed so far. Add what yours uses;
  contributions welcome.

## Used by

- [OpenNextion Room Companion](https://github.com/fixtse/OpenNextion-Room-Companion)
  (`emulator/`): renders the 2.8" and 3.5" panels from the firmware's own
  headers and board profiles.

## License

MIT. LVGL (MIT), ArduinoJson (MIT) and lodepng (zlib) are fetched at build
time under their own licenses.
