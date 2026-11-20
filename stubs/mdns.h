#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>
#define ESP_IPADDR_TYPE_V4 0
#define IPSTR "%d.%d.%d.%d"
#define IP2STR(a) 0, 0, 0, 0
#define MDNS_TYPE_PTR 0x000C
struct esp_ip4_addr_t { uint32_t addr; };
struct esp_ip_addr_t { union { esp_ip4_addr_t ip4; } u_addr; uint8_t type; };
struct mdns_ip_addr_t { esp_ip_addr_t addr; mdns_ip_addr_t *next; };
struct mdns_txt_item_t { const char *key, *value; };
struct mdns_result_t { mdns_result_t *next; char *instance_name, *hostname; uint16_t port; mdns_txt_item_t *txt; uint8_t *txt_value_len; size_t txt_count; mdns_ip_addr_t *addr; };
struct mdns_search_once_t;
inline mdns_search_once_t *mdns_query_async_new(const char *, const char *, const char *, uint16_t, uint32_t, size_t, void *) { return nullptr; }
inline bool mdns_query_async_get_results(mdns_search_once_t *, uint32_t, mdns_result_t **, uint8_t *) { return false; }
inline void mdns_query_async_delete(mdns_search_once_t *) {}
inline void mdns_query_results_free(mdns_result_t *) {}
