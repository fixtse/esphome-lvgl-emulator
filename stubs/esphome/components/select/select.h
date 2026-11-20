#pragma once
#include <cstddef>
#include <cstring>
#include <optional>
#include <string>
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
namespace esphome::select {
class SelectTraits {
 public:
  void set_options(const FixedVector<const char *> &o) { options_ = o; }
  const FixedVector<const char *> &get_options() const { return options_; }
 private:
  FixedVector<const char *> options_;
};
class Select : public Component {
 public:
  std::string state;
  SelectTraits traits;
  void publish_state(const std::string &s) { state = s; }
  void publish_state(const char *s) { state = s ? s : ""; }
  void publish_state(size_t i) { if (i < traits.get_options().size()) state = traits.get_options()[i]; }
  StringRef current_option() const { return StringRef(state); }
  bool has_option(const std::string &o) const { return index_of(o).has_value(); }
  std::optional<size_t> index_of(const std::string &o) const {
    const auto &v = traits.get_options();
    for (size_t i = 0; i < v.size(); ++i) if (o == v[i]) return i;
    return std::nullopt;
  }
  std::optional<size_t> active_index() const { return index_of(state); }
  bool has_state() const { return !state.empty(); }
  size_t size() const { return traits.get_options().size(); }
};
}  // namespace esphome::select
