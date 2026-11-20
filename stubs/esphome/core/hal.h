#pragma once
// Host stand-in: time is driven by the emulator (emu::now_ms).
#include <cstdint>
namespace emu { inline uint32_t now_ms = 100000; }
namespace esphome {
inline uint32_t millis() { return emu::now_ms; }
inline uint32_t micros() { return emu::now_ms * 1000u; }
inline void delay(uint32_t) {}
inline void yield() {}
}
