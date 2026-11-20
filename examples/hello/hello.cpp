// Minimal emulator app: a 240x320 thermostat-style screen built with plain
// LVGL calls, as an ESPHome lambda or custom component would build it.
//
//   build/hello-panel --out out            # PNGs
//   build/hello-panel --sdl                # window (needs a display)
//   build/hello-panel --width 320 --height 480 --out out
#include "lvgl_emu/runtime.h"
#include "esphome.h"  // ESPHome stand-ins: millis(), ESP_LOG*, preferences, ...

namespace {
const char *FONT = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
lv_obj_t *arc = nullptr, *value = nullptr, *state = nullptr;

void build(int w, int h) {
  lv_obj_t *scr = lv_screen_active();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x101418), 0);
  lv_obj_t *title = lv_label_create(scr);
  lv_obj_set_style_text_font(title, lvgl_emu::ttf(FONT, h / 18), 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(title, "Living room");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, h / 20);

  arc = lv_arc_create(scr);
  lv_obj_set_size(arc, w * 3 / 4, w * 3 / 4);
  lv_arc_set_range(arc, 10, 30);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0xFF9F0A), LV_PART_INDICATOR);
  lv_obj_align(arc, LV_ALIGN_CENTER, 0, -h / 20);
  value = lv_label_create(scr);
  lv_obj_set_style_text_font(value, lvgl_emu::ttf(FONT, h / 8), 0);
  lv_obj_set_style_text_color(value, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align_to(value, arc, LV_ALIGN_CENTER, 0, 0);
  state = lv_label_create(scr);
  lv_obj_set_style_text_font(state, lvgl_emu::ttf(FONT, h / 22), 0);
  lv_obj_set_style_text_color(state, lv_color_hex(0x9AA4AE), 0);
  lv_obj_align(state, LV_ALIGN_BOTTOM_MID, 0, -h / 12);
}

void show(int target, const char *mode) {
  lv_arc_set_value(arc, target);
  lv_label_set_text_fmt(value, "%d°", target);
  lv_obj_align_to(value, arc, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text(state, mode);
  ESP_LOGW("hello", "showing %d° (%s) at %u ms", target, mode, (unsigned) esphome::millis());
}
}  // namespace

lvgl_emu::App lvgl_emu_app(const lvgl_emu::Args &args) {
  lvgl_emu::App app;
  app.width = std::stoi(args.get("width", "240"));
  app.height = std::stoi(args.get("height", "320"));
  app.name = "hello-" + std::to_string(app.width) + "x" + std::to_string(app.height);
  app.setup = [w = app.width, h = app.height]() { build(w, h); };
  app.screens = []() {
    return std::vector<lvgl_emu::Screen>{{"heating", [] { show(22, "Heating"); }}, {"idle", [] { show(18, "Idle"); }}};
  };
  return app;
}
