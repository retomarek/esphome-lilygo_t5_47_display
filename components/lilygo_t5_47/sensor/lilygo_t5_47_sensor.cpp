#include "lilygo_t5_47_sensor.h"
#include "esphome/core/log.h"

extern "C" {
#include "eink.h"
}

namespace esphome {
namespace lilygo_t5_47 {

static const char *const TAG = "lilygo_t5_47.sensor";

static int correct_adc_reference() {
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    return adc_chars.vref;
  }
  return 1100;
}

float LilygoT547Sensor::get_battery_voltage() {
  int vref = correct_adc_reference();
  int v = analogRead(36);
  return ((float) v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
}

void LilygoT547Sensor::update() {
  eink_power_on();
  delay(20);
  this->publish_state(this->get_battery_voltage());
  eink_power_off();
}

float LilygoT547Sensor::get_setup_priority() const { return esphome::setup_priority::LATE; }

void LilygoT547Sensor::dump_config() {
  LOG_SENSOR("", "Battery Voltage Sensor", this);
  LOG_UPDATE_INTERVAL(this);
}

}  // namespace lilygo_t5_47
}  // namespace esphome
