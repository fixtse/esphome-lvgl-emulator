#pragma once
#include "lvgl.h"
namespace esphome::image {
class Image {
 public:
  explicit Image(const lv_image_dsc_t *dsc = nullptr) : dsc_(dsc) {}
  const lv_image_dsc_t *get_lv_image_dsc() { return dsc_; }
  int get_width() const { return dsc_ ? dsc_->header.w : 0; }
  int get_height() const { return dsc_ ? dsc_->header.h : 0; }
 private:
  const lv_image_dsc_t *dsc_;
};
}  // namespace esphome::image
