#pragma once
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <string>
namespace esphome {
struct ESPTime {
  uint8_t second{0}, minute{0}, hour{0}, day_of_week{1}, day_of_month{1};
  uint16_t day_of_year{1};
  uint8_t month{1};
  uint16_t year{1970};
  bool is_dst{false};
  time_t timestamp{0};
  bool is_valid() const { return year >= 2019; }
  static ESPTime from_c_tm(const struct tm *t, time_t ts) {
    ESPTime r;
    r.second = t->tm_sec; r.minute = t->tm_min; r.hour = t->tm_hour; r.day_of_week = t->tm_wday + 1;
    r.day_of_month = t->tm_mday; r.day_of_year = t->tm_yday + 1; r.month = t->tm_mon + 1; r.year = t->tm_year + 1900;
    r.is_dst = t->tm_isdst > 0; r.timestamp = ts;
    return r;
  }
  static ESPTime from_epoch_local(time_t ts) { struct tm t; localtime_r(&ts, &t); return from_c_tm(&t, ts); }
  static ESPTime from_epoch_utc(time_t ts) { struct tm t; gmtime_r(&ts, &t); return from_c_tm(&t, ts); }
  static bool strptime(const std::string &s, ESPTime &out) {
    int y, mo, d, h = 0, mi = 0, se = 0;
    const int n = std::sscanf(s.c_str(), "%d-%d-%d %d:%d:%d", &y, &mo, &d, &h, &mi, &se);
    if (n < 3) return false;
    out.year = y; out.month = mo; out.day_of_month = d; out.hour = h; out.minute = mi; out.second = se;
    return true;
  }
  void recalc_timestamp_utc(bool = true) {
    struct tm t{}; t.tm_year = year - 1900; t.tm_mon = month - 1; t.tm_mday = day_of_month;
    t.tm_hour = hour; t.tm_min = minute; t.tm_sec = second; timestamp = timegm(&t);
  }
  void recalc_timestamp_local() {
    struct tm t{}; t.tm_year = year - 1900; t.tm_mon = month - 1; t.tm_mday = day_of_month;
    t.tm_hour = hour; t.tm_min = minute; t.tm_sec = second; t.tm_isdst = -1; timestamp = mktime(&t);
  }
  std::string strftime(const char *fmt) const {
    struct tm t{}; t.tm_year = year - 1900; t.tm_mon = month - 1; t.tm_mday = day_of_month; t.tm_hour = hour;
    t.tm_min = minute; t.tm_sec = second; t.tm_wday = day_of_week - 1; t.tm_yday = day_of_year - 1;
    char buf[128]; size_t n = ::strftime(buf, sizeof(buf), fmt, &t); return std::string(buf, n);
  }
};
}  // namespace esphome
