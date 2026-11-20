#include "lvgl_emu/runtime.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <regex>
#include <sstream>
#include <unistd.h>
#include "esphome/core/hal.h"
#define LODEPNG_NO_COMPILE_CPP
#include "src/libs/lodepng/lodepng.h"

namespace lvgl_emu {
namespace {
std::vector<uint16_t> frame;
int frame_w = 0, frame_h = 0;
std::map<std::string, lv_font_t *> font_cache;
std::map<std::string, std::string> image_cache;
std::string cache_dir;

std::string read_file(const std::string &path) {
  std::ifstream in(path, std::ios::binary);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
void flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px) {
  const auto *src = reinterpret_cast<const uint16_t *>(px);
  const int w = lv_area_get_width(area);
  for (int y = area->y1; y <= area->y2; ++y)
    for (int x = area->x1; x <= area->x2; ++x) frame[y * frame_w + x] = src[(y - area->y1) * w + (x - area->x1)];
  lv_display_flush_ready(disp);
}
void advance(uint32_t ms) {
  emu::now_ms += ms;
  lv_tick_inc(ms);
}
std::string safe_name(std::string s) {
  for (auto &c : s)
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_') c = '_';
  return s;
}
}  // namespace

const lv_font_t *ttf(const std::string &path, int size) {
  const std::string key = path + "@" + std::to_string(size);
  if (auto it = font_cache.find(key); it != font_cache.end()) return it->second;
  lv_font_t *f = lv_freetype_font_create(path.c_str(), LV_FREETYPE_FONT_RENDER_MODE_BITMAP, size, LV_FREETYPE_FONT_STYLE_NORMAL);
  if (f == nullptr) {
    std::fprintf(stderr, "lvgl_emu: cannot load font %s\n", path.c_str());
    return &lv_font_montserrat_14;
  }
  return font_cache[key] = f;
}

// LVGL decodes PNG files itself (lodepng + stdio driver "A:"); SVGs are first
// rasterised to PNG at the size the ESPHome image component would resize to.
const void *svg(const std::string &path, int width, int height) {
  if (height <= 0) height = width;
  const std::string key = path + "@" + std::to_string(width) + "x" + std::to_string(height);
  if (auto it = image_cache.find(key); it != image_cache.end()) return it->second.empty() ? nullptr : it->second.c_str();
  const std::string file = cache_dir + "/" + std::to_string(image_cache.size()) + ".png";
  std::string source = path;
  // Raster images (PNG/JPEG) are resized the same way, through an SVG wrapper.
  const auto ext = path.substr(path.find_last_of('.') + 1);
  if (ext == "png" || ext == "jpg" || ext == "jpeg") {
    const std::string bytes = read_file(path);
    static const char *t = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string b64;
    for (size_t i = 0; i < bytes.size(); i += 3) {
      const uint32_t v = uint8_t(bytes[i]) << 16 | (i + 1 < bytes.size() ? uint8_t(bytes[i + 1]) << 8 : 0) |
                         (i + 2 < bytes.size() ? uint8_t(bytes[i + 2]) : 0);
      b64 += t[v >> 18 & 63]; b64 += t[v >> 12 & 63];
      b64 += i + 1 < bytes.size() ? t[v >> 6 & 63] : '='; b64 += i + 2 < bytes.size() ? t[v & 63] : '=';
    }
    source = file + ".svg";
    std::ofstream wrapper(source);
    wrapper << "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='" << width
            << "' height='" << height << "'><image width='" << width << "' height='" << height
            << "' preserveAspectRatio='none' xlink:href='data:image/" << (ext == "png" ? "png" : "jpeg") << ";base64," << b64
            << "'/></svg>";
  }
  const std::string cmd = "rsvg-convert -w " + std::to_string(width) + " -h " + std::to_string(height) + " -o '" + file +
                          "' '" + source + "' 2>/dev/null";
  if (std::system(cmd.c_str()) != 0) {
    std::fprintf(stderr, "lvgl_emu: cannot rasterise %s\n", path.c_str());
    image_cache[key] = "";
    return nullptr;
  }
  return (image_cache[key] = "A:" + file).c_str();
}
const void *png(const std::string &path) {
  if (auto it = image_cache.find(path); it != image_cache.end()) return it->second.c_str();
  return (image_cache[path] = "A:" + path).c_str();
}

void settle(uint32_t ms) {
  for (uint32_t t = 0; t < ms; t += 10) {
    advance(10);
    lv_timer_handler();
  }
  lv_obj_invalidate(lv_screen_active());
  lv_refr_now(nullptr);
}

bool write_png(const std::string &path) {
  if (frame.empty()) return false;
  std::vector<uint8_t> rgb(frame_w * frame_h * 3);
  for (int i = 0; i < frame_w * frame_h; ++i) {
    const uint16_t c = frame[i];
    rgb[i * 3] = (c >> 11) * 255 / 31;
    rgb[i * 3 + 1] = ((c >> 5) & 0x3F) * 255 / 63;
    rgb[i * 3 + 2] = (c & 0x1F) * 255 / 31;
  }
  unsigned char *data = nullptr;
  size_t size = 0;
  if (lodepng_encode24(&data, &size, rgb.data(), frame_w, frame_h) != 0) return false;
  std::ofstream file(path, std::ios::binary);
  file.write(reinterpret_cast<const char *>(data), size);
  lv_free(data);
  return bool(file);
}

uint32_t now_ms() { return emu::now_ms; }

std::map<std::string, std::string> substitutions(const std::string &yaml_path, std::map<std::string, std::string> out) {
  std::istringstream lines(read_file(yaml_path));
  std::string line;
  const std::regex entry(R"re(^\s{2}([A-Za-z0-9_]+):\s*"?([^"#]*?)"?\s*(#.*)?$)re");
  bool in_subs = false;
  while (std::getline(lines, line)) {
    if (line.rfind("substitutions:", 0) == 0) { in_subs = true; continue; }
    if (!line.empty() && line[0] != ' ' && line[0] != '#') in_subs = false;
    std::smatch m;
    if (in_subs && std::regex_match(line, m, entry)) out[m[1]] = m[2];
  }
  return out;
}
}  // namespace lvgl_emu

namespace {
void usage() {
  std::fprintf(stderr,
               "usage: APP [--out DIR] [--sdl] [--interval MS] [--only TEXT] [--KEY VALUE ...]\n"
               "  Renders each screen to DIR/<name>.png, or shows them in an SDL window.\n");
}
}  // namespace

int main(int argc, char **argv) {
  lvgl_emu::Args args;
  for (int i = 1; i < argc; ++i) {
    const std::string a = argv[i];
    auto value = [&]() -> std::string {
      if (i + 1 >= argc) { usage(); std::exit(2); }
      return argv[++i];
    };
    if (a == "--help" || a == "-h") { usage(); return 0; }
    if (a == "--sdl") args.sdl = true;
    else if (a == "--out") args.out = value();
    else if (a == "--interval") args.interval_ms = std::stoul(value());
    else if (a == "--only") args.only = value();
    else if (a.rfind("--", 0) == 0) args.extra[a.substr(2)] = value();
    else { usage(); return 2; }
  }
  // Reproducible output: UTC unless the caller sets TZ.
  if (getenv("TZ") == nullptr) setenv("TZ", "UTC", 1);
  tzset();

  lvgl_emu::App app = lvgl_emu_app(args);
  char tmpl[] = "/tmp/lvgl-emu-XXXXXX";
  lvgl_emu::cache_dir = mkdtemp(tmpl) ? tmpl : "/tmp";

  lv_init();
  lv_freetype_init(64);
  lv_display_t *disp = nullptr;
#if LV_USE_SDL
  if (args.sdl) {
    disp = lv_sdl_window_create(app.width, app.height);
    lv_sdl_mouse_create();  // The mouse acts as the touchscreen.
  }
#else
  if (args.sdl) std::fprintf(stderr, "built without SDL; rendering PNGs instead\n");
  args.sdl = false;
#endif
  static std::vector<uint16_t> draw_buffer;
  if (disp == nullptr) {
    args.sdl = false;
    disp = lv_display_create(app.width, app.height);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    draw_buffer.resize(app.width * app.height);
    lvgl_emu::frame.assign(app.width * app.height, 0);
    lvgl_emu::frame_w = app.width;
    lvgl_emu::frame_h = app.height;
    lv_display_set_buffers(disp, draw_buffer.data(), nullptr, draw_buffer.size() * 2, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, lvgl_emu::flush);
  }
  if (app.setup) app.setup();
  std::vector<lvgl_emu::Screen> screens = app.screens ? app.screens() : std::vector<lvgl_emu::Screen>{};
  if (!args.only.empty())
    std::erase_if(screens, [&](const auto &s) { return s.name.find(args.only) == std::string::npos; });
  if (screens.empty()) { std::fprintf(stderr, "no screens to render\n"); return 1; }

  if (args.sdl) {
    size_t index = 0;
    uint32_t shown = 0;
    screens[index].show();
    std::printf("%s\n", screens[index].name.c_str());
    while (true) {
      lv_timer_handler();
      usleep(10000);
      lvgl_emu::advance(10);
      if ((shown += 10) >= args.interval_ms) {
        shown = 0;
        index = (index + 1) % screens.size();
        screens[index].show();
        std::printf("%s\n", screens[index].name.c_str());
      }
    }
  }
  std::system(("mkdir -p '" + args.out + "'").c_str());
  int failures = 0, n = 0;
  for (const auto &screen : screens) {
    screen.show();
    lvgl_emu::settle();
    char index[8];
    std::snprintf(index, sizeof(index), "%02d", ++n);
    const std::string path = args.out + "/" + (app.name.empty() ? "" : app.name + "-") + index + "-" +
                             lvgl_emu::safe_name(screen.name) + ".png";
    const bool ok = lvgl_emu::write_png(path);
    failures += !ok;
    std::printf("%s %s\n", ok ? "wrote" : "FAILED", path.c_str());
  }
  std::system(("rm -rf '" + lvgl_emu::cache_dir + "'").c_str());
  return failures ? 1 : 0;
}
