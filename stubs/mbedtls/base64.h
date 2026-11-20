#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#define MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL -0x002A
#define MBEDTLS_ERR_BASE64_INVALID_CHARACTER -0x002C
inline int mbedtls_base64_encode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen) {
  static const char *t = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  const size_t need = (slen + 2) / 3 * 4;
  *olen = need + 1;
  if (dst == nullptr || dlen < need + 1) return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
  size_t o = 0;
  for (size_t i = 0; i < slen; i += 3) {
    uint32_t v = src[i] << 16 | (i + 1 < slen ? src[i + 1] << 8 : 0) | (i + 2 < slen ? src[i + 2] : 0);
    dst[o++] = t[v >> 18 & 63]; dst[o++] = t[v >> 12 & 63];
    dst[o++] = i + 1 < slen ? t[v >> 6 & 63] : '='; dst[o++] = i + 2 < slen ? t[v & 63] : '=';
  }
  dst[o] = 0; *olen = o; return 0;
}
inline int mbedtls_base64_decode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen) {
  auto val = [](unsigned char c) -> int { return c >= 'A' && c <= 'Z' ? c - 'A' : c >= 'a' && c <= 'z' ? c - 'a' + 26 : c >= '0' && c <= '9' ? c - '0' + 52 : c == '+' ? 62 : c == '/' ? 63 : -1; };
  size_t o = 0; uint32_t acc = 0; int bits = 0;
  for (size_t i = 0; i < slen; ++i) {
    if (src[i] == '=' || src[i] == '\n' || src[i] == '\r') continue;
    const int v = val(src[i]); if (v < 0) return MBEDTLS_ERR_BASE64_INVALID_CHARACTER;
    acc = acc << 6 | v; bits += 6;
    if (bits >= 8) { bits -= 8; if (dst) { if (o >= dlen) return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL; dst[o] = acc >> bits & 0xFF; } ++o; }
  }
  *olen = o; return dst ? 0 : MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
}
