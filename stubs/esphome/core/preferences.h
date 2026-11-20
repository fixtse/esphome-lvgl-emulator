#pragma once
// In-memory preferences: values persist for the emulator run only.
#include <cstdint>
#include <cstring>
#include <map>
#include <memory>
#include <vector>
namespace esphome {
class ESPPreferenceObject {
 public:
  ESPPreferenceObject() = default;
  explicit ESPPreferenceObject(std::vector<uint8_t> *slot) : slot_(slot) {}
  template<typename T> bool save(const T *src) {
    if (!slot_) return false;
    slot_->assign(reinterpret_cast<const uint8_t *>(src), reinterpret_cast<const uint8_t *>(src) + sizeof(T));
    return true;
  }
  template<typename T> bool load(T *dest) {
    if (!slot_ || slot_->size() != sizeof(T)) return false;
    std::memcpy(dest, slot_->data(), sizeof(T));
    return true;
  }
 private:
  std::vector<uint8_t> *slot_{nullptr};
};
class ESPPreferences {
 public:
  template<typename T> ESPPreferenceObject make_preference(uint32_t key, bool = false) { return ESPPreferenceObject(&store_[key]); }
  bool sync() { return true; }
  bool reset() { store_.clear(); return true; }
 private:
  std::map<uint32_t, std::vector<uint8_t>> store_;
};
inline ESPPreferences *global_preferences = new ESPPreferences();
}  // namespace esphome
