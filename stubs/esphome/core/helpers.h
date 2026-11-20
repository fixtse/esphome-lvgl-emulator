#pragma once
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>
namespace esphome {
template<typename T> using optional = std::optional<T>;
inline constexpr std::nullopt_t nullopt = std::nullopt;
inline uint32_t fnv1_hash(const char *s) {
  uint32_t h = 2166136261UL;
  for (; s && *s; ++s) { h *= 16777619UL; h ^= uint8_t(*s); }
  return h;
}
inline uint32_t fnv1_hash(const std::string &s) { return fnv1_hash(s.c_str()); }
template<class T> class RAMAllocator {
 public:
  using value_type = T;
  enum Flags { NONE = 0, ALLOC_EXTERNAL = 1, ALLOC_INTERNAL = 2, ALLOW_FAILURE = 4 };
  RAMAllocator() = default;
  RAMAllocator(uint8_t) {}
  template<class U> constexpr RAMAllocator(const RAMAllocator<U> &) {}
  T *allocate(size_t n) { return static_cast<T *>(std::malloc(n * sizeof(T))); }
  T *reallocate(T *p, size_t n) { return static_cast<T *>(std::realloc(p, n * sizeof(T))); }
  void deallocate(T *p, size_t = 0) { std::free(p); }
  size_t get_free_heap_size() const { return 8u << 20; }
  size_t get_max_free_block_size() const { return 4u << 20; }
  template<class U> bool operator==(const RAMAllocator<U> &) const { return true; }
  template<class U> bool operator!=(const RAMAllocator<U> &) const { return false; }
};
template<class T> using ExternalRAMAllocator = RAMAllocator<T>;
class Mutex {
 public:
  void lock() { m_.lock(); }
  void unlock() { m_.unlock(); }
  bool try_lock() { return m_.try_lock(); }
 private:
  std::recursive_mutex m_;
};
class LockGuard {
 public:
  explicit LockGuard(Mutex &m) : m_(m) { m_.lock(); }
  ~LockGuard() { m_.unlock(); }
 private:
  Mutex &m_;
};
template<typename T> class FixedVector {
 public:
  void init(size_t n) { v_.clear(); v_.reserve(n); }
  void push_back(const T &t) { v_.push_back(t); }
  template<typename... A> T &emplace_back(A &&...a) { return v_.emplace_back(std::forward<A>(a)...); }
  size_t size() const { return v_.size(); }
  bool empty() const { return v_.empty(); }
  T &operator[](size_t i) { return v_[i]; }
  const T &operator[](size_t i) const { return v_[i]; }
  auto begin() { return v_.begin(); }
  auto end() { return v_.end(); }
  auto begin() const { return v_.begin(); }
  auto end() const { return v_.end(); }
 private:
  std::vector<T> v_;
};
class StringRef {
 public:
  StringRef() = default;
  StringRef(const char *s) : s_(s ? s : "") {}
  StringRef(const std::string &s) : s_(s) {}
  const char *c_str() const { return s_.c_str(); }
  const char *data() const { return s_.data(); }
  size_t size() const { return s_.size(); }
  bool empty() const { return s_.empty(); }
  operator std::string() const { return s_; }
  std::string str() const { return s_; }
  bool operator==(const char *o) const { return s_ == o; }
  bool operator==(const std::string &o) const { return s_ == o; }
 private:
  std::string s_;
};
// Fixed, so renders are reproducible.
inline std::string get_mac_address() { return "000000000000"; }
inline std::string str_sprintf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
inline std::string str_sprintf(const char *fmt, ...) {
  char buf[512];
  va_list a; va_start(a, fmt); vsnprintf(buf, sizeof(buf), fmt, a); va_end(a);
  return buf;
}
template<typename T> T clamp(T v, T lo, T hi) { return v < lo ? lo : hi < v ? hi : v; }
inline uint32_t random_uint32() { return uint32_t(std::rand()); }
}  // namespace esphome

