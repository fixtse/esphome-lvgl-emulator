#pragma once
// In-memory NVS for the emulator (blobs only, as the panel uses).
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "esp_err.h"
typedef uint32_t nvs_handle_t;
typedef enum { NVS_READONLY, NVS_READWRITE } nvs_open_mode_t;
typedef enum { NVS_TYPE_BLOB = 0x42, NVS_TYPE_ANY = 0xff } nvs_type_t;
typedef struct { char namespace_name[16]; char key[16]; nvs_type_t type; } nvs_entry_info_t;
typedef struct { size_t used_entries, free_entries, available_entries, total_entries, namespace_count; } nvs_stats_t;
namespace emu_nvs {
inline std::vector<std::string> spaces;
inline std::map<std::pair<std::string, std::string>, std::vector<uint8_t>> blobs;
inline std::string space(nvs_handle_t h) { return h > 0 && h <= spaces.size() ? spaces[h - 1] : std::string(); }
struct Iterator { std::string space; std::map<std::pair<std::string, std::string>, std::vector<uint8_t>>::iterator at; };
}
typedef emu_nvs::Iterator *nvs_iterator_t;
inline esp_err_t nvs_open(const char *name, nvs_open_mode_t, nvs_handle_t *out) {
  emu_nvs::spaces.push_back(name); *out = emu_nvs::spaces.size(); return ESP_OK;
}
inline void nvs_close(nvs_handle_t) {}
inline esp_err_t nvs_commit(nvs_handle_t) { return ESP_OK; }
inline esp_err_t nvs_set_blob(nvs_handle_t h, const char *key, const void *v, size_t n) {
  auto &b = emu_nvs::blobs[{emu_nvs::space(h), key}];
  b.assign(static_cast<const uint8_t *>(v), static_cast<const uint8_t *>(v) + n); return ESP_OK;
}
inline esp_err_t nvs_get_blob(nvs_handle_t h, const char *key, void *out, size_t *n) {
  auto it = emu_nvs::blobs.find({emu_nvs::space(h), key});
  if (it == emu_nvs::blobs.end()) return ESP_ERR_NVS_NOT_FOUND;
  if (out == nullptr) { *n = it->second.size(); return ESP_OK; }
  if (*n < it->second.size()) return ESP_ERR_NVS_INVALID_LENGTH;
  std::memcpy(out, it->second.data(), it->second.size()); *n = it->second.size(); return ESP_OK;
}
inline esp_err_t nvs_erase_key(nvs_handle_t h, const char *key) {
  return emu_nvs::blobs.erase({emu_nvs::space(h), key}) ? ESP_OK : ESP_ERR_NVS_NOT_FOUND;
}
inline esp_err_t nvs_get_stats(const char *, nvs_stats_t *s) { *s = {emu_nvs::blobs.size(), 1000, 1000, 1000, 1}; return ESP_OK; }
inline esp_err_t nvs_entry_find(const char *, const char *space, nvs_type_t, nvs_iterator_t *out) {
  auto *it = new emu_nvs::Iterator{space ? space : "", emu_nvs::blobs.begin()};
  while (it->at != emu_nvs::blobs.end() && it->at->first.first != it->space) ++it->at;
  if (it->at == emu_nvs::blobs.end()) { delete it; *out = nullptr; return ESP_ERR_NVS_NOT_FOUND; }
  *out = it; return ESP_OK;
}
inline esp_err_t nvs_entry_next(nvs_iterator_t *it) {
  auto *i = *it; ++i->at;
  while (i->at != emu_nvs::blobs.end() && i->at->first.first != i->space) ++i->at;
  if (i->at == emu_nvs::blobs.end()) { delete i; *it = nullptr; return ESP_ERR_NVS_NOT_FOUND; }
  return ESP_OK;
}
inline esp_err_t nvs_entry_info(nvs_iterator_t it, nvs_entry_info_t *info) {
  std::snprintf(info->namespace_name, sizeof(info->namespace_name), "%s", it->at->first.first.c_str());
  std::snprintf(info->key, sizeof(info->key), "%s", it->at->first.second.c_str());
  info->type = NVS_TYPE_BLOB; return ESP_OK;
}
inline void nvs_release_iterator(nvs_iterator_t it) { delete it; }
#include <cstdio>
