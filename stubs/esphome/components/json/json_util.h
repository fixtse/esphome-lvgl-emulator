#pragma once
#include <ArduinoJson.h>
#include <cstdlib>
namespace esphome::json {
struct SpiRamAllocator : ArduinoJson::Allocator {
  void *allocate(size_t n) override { return std::malloc(n); }
  void deallocate(void *p) override { std::free(p); }
  void *reallocate(void *p, size_t n) override { return std::realloc(p, n); }
};
}  // namespace esphome::json
