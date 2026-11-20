#pragma once
// esphome-lvgl-emulator: run ESP32/ESPHome LVGL UI code on a PC.
//
// An app links against this runtime and defines lvgl_emu_app(). The runtime
// parses the common options, creates the LVGL display (an SDL window or an
// off-screen framebuffer), calls the app's setup, then renders each screen
// the app lists to a PNG, or cycles through them in the window.
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include "lvgl.h"

namespace lvgl_emu {

// One screen to capture: show() builds or updates the LVGL objects for it.
struct Screen {
  std::string name;
  std::function<void()> show;
};

struct App {
  int width{240}, height{320};
  // Called once after lv_init() and display creation, before any screen.
  std::function<void()> setup;
  // The screens to render, in order.
  std::function<std::vector<Screen>()> screens;
  // Prefix for PNG file names (default: empty).
  std::string name;
};

// Options the runtime parses. Unknown `--key value` pairs are passed to the
// app in `extra` (for example --model or --scenario).
struct Args {
  std::string out{"out"};
  bool sdl{false};
  uint32_t interval_ms{4000};      // SDL: time per screen.
  std::string only;                // Render only screens whose name contains this.
  std::map<std::string, std::string> extra;
  std::string get(const std::string &key, const std::string &fallback = "") const {
    auto it = extra.find(key);
    return it == extra.end() ? fallback : it->second;
  }
};

// ------------------------------------------------------------ helpers for apps
// Loads a TrueType font with FreeType at a pixel size (cached), as ESPHome's
// font component renders it. Returns lv_font_montserrat_14 on failure.
const lv_font_t *ttf(const std::string &path, int size);
// Rasterises an SVG (rsvg-convert) or loads a PNG at the given size and
// returns an LVGL image source (cached). nullptr on failure.
const void *svg(const std::string &path, int width, int height = 0);
const void *png(const std::string &path);
// Runs timers and animations for `ms` of emulated time, then redraws fully.
void settle(uint32_t ms = 2000);
// Writes the current framebuffer (off-screen mode) as a PNG.
bool write_png(const std::string &path);
// Emulated milliseconds since start; also what esphome::millis() returns.
uint32_t now_ms();
// Reads `key: value` entries under `substitutions:` from an ESPHome YAML file.
std::map<std::string, std::string> substitutions(const std::string &yaml_path,
                                                 std::map<std::string, std::string> into = {});

}  // namespace lvgl_emu

// Defined by the app.
lvgl_emu::App lvgl_emu_app(const lvgl_emu::Args &args);
