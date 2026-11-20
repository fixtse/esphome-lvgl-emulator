#pragma once
#include <functional>
#include <string>
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
namespace esphome {
namespace setup_priority { inline constexpr float WIFI = 250.0f, LATE = -100.0f, DATA = 600.0f, HARDWARE = 800.0f, PROCESSOR = 400.0f; }
class Component {
 public:
  virtual ~Component() = default;
  virtual void setup() {}
  virtual void loop() {}
  virtual void dump_config() {}
  virtual float get_setup_priority() const { return 0; }
  void mark_failed() {}
  bool is_failed() const { return false; }
  void disable_loop() {}
  void enable_loop() {}
  void set_timeout(const std::string &, uint32_t, std::function<void()> f) { if (f) f(); }
  void set_timeout(uint32_t, std::function<void()> f) { if (f) f(); }
  bool cancel_timeout(const std::string &) { return true; }
  void set_interval(const std::string &, uint32_t, std::function<void()>) {}
  bool cancel_interval(const std::string &) { return true; }
  void defer(std::function<void()> f) { if (f) f(); }
  void status_set_warning(const char * = nullptr) {}
  void status_clear_warning() {}
};
class PollingComponent : public Component {
 public:
  PollingComponent() = default;
  explicit PollingComponent(uint32_t) {}
  virtual void update() {}
};
struct Application { void feed_wdt() {} };
inline Application App;
}  // namespace esphome
