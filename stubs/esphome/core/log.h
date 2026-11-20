#pragma once
#include <cstdio>
#define ESP_LOGE(tag, ...) (std::fprintf(stderr, "[E][%s] ", tag), std::fprintf(stderr, __VA_ARGS__), std::fputc('\n', stderr))
#define ESP_LOGW(tag, ...) (std::fprintf(stderr, "[W][%s] ", tag), std::fprintf(stderr, __VA_ARGS__), std::fputc('\n', stderr))
#define ESP_LOGI(tag, ...) ((void) 0)
#define ESP_LOGD(tag, ...) ((void) 0)
#define ESP_LOGV(tag, ...) ((void) 0)
#define ESP_LOGVV(tag, ...) ((void) 0)
#define ESP_LOGCONFIG(tag, ...) ((void) 0)
