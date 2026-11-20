#pragma once
namespace esphome::globals {
template<typename T> class GlobalsComponent {
 public:
  explicit GlobalsComponent(T v) : value_(v) {}
  T &value() { return value_; }
 private:
  T value_;
};
}  // namespace esphome::globals
// ESPHome lambdas read globals through id(); declare yours as
//   inline auto *my_global = new esphome::globals::GlobalsComponent<int>(0);
template<typename T> T &id(esphome::globals::GlobalsComponent<T> *g) { return g->value(); }
