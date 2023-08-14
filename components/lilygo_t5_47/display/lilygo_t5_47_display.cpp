#include "lilygo_t5_47_display.h"

#include "esphome/core/log.h"
#include <string.h>
extern "C" {
#include "eink.h"
}

namespace esphome {
namespace lilygo_t5_47 {

static const char *const TAG = "lilygo_t5_47.display";

// Assuming there will be at least one display instance at a time
// Static initialization to save time on memory allocation in runtime,
// ans also workaround framgamted memory in ESPHome
static uint8_t fb[EINK_BUFFER_SIZE] = {0};

RTC_NOINIT_ATTR static uint32_t full_update_countdown_;

static inline bool convert_color(Color color) { return color.red != 0 || color.green != 0 || color.blue != 0; }

float LilygoT547Display::get_setup_priority() const { return esphome::setup_priority::LATE; }

int LilygoT547Display::get_width_internal() { return EINK_WIDTH; }

int LilygoT547Display::get_height_internal() { return EINK_HEIGHT; }

void LilygoT547Display::fill(Color color) { eink_buffer_set(fb, convert_color(color)); }

void LilygoT547Display::setup() {
  eink_init();

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED || full_update_countdown_ < 0 || full_update_countdown_ >= this->full_update_every_) {
    ESP_LOGD(TAG, "Resetting periodic full update countdown to 0.");
    full_update_countdown_ = 0;
  } else {
    ESP_LOGD(TAG, "Restored periodic full update countdown to %u.", full_update_countdown_);
  }
}

void LilygoT547Display::update() {
  this->do_update_();
  LilygoT547Display::flush_screen_changes();
}

void LilygoT547Display::flush_screen_changes() {
  eink_power_on();

  if (this->full_update_every_ > 0) {
    ESP_LOGD(TAG, "Periodic full update countdown %u.", full_update_countdown_);

    if (full_update_countdown_ == 0) {
      full_update_countdown_ = this->full_update_every_;
      ESP_LOGD(TAG, "Full update!");
      
      eink_render_advanced(fb, this->cycles_invert_, true);
    }

    full_update_countdown_--;
  }

  eink_render_advanced(fb, this->cycles_render_, false);

  eink_power_off();
}

void LilygoT547Display::on_shutdown() { eink_deinit(); }

void HOT LilygoT547Display::draw_absolute_pixel_internal(int x, int y, Color color) {
  bool c = convert_color(color);
  eink_set_pixel(x, y, c, fb);
}

void LilygoT547Display::power_on() { eink_power_on(); }

void LilygoT547Display::power_off() { eink_power_off(); }

void LilygoT547Display::dump_config() {
  LOG_DISPLAY("", "Lilygo T5 47 Display", this);
  ESP_LOGCONFIG(TAG, "  Full Update Every: %u", this->full_update_every_);
  LOG_UPDATE_INTERVAL(this);
}

void LilygoT547Display::set_full_update_countdown(uint32_t value) { full_update_countdown_ = value; }
uint32_t LilygoT547Display::get_full_update_countdown() { return full_update_countdown_; }

}  // namespace lilygo_t5_47
}  // namespace esphome
