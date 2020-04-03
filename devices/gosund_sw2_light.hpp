#pragma once
#include "esphome.h"

namespace esphome {
  namespace gosund {

    class GosundLight : public Component, public light::LightOutput, public mqtt::CustomMQTTDevice {
    public:
      GosundLight(output::BinaryOutput *status_led) { status_led_ = status_led; }
      void setup() override;
      void loop() override;
      void dump_config() override;
      light::LightTraits get_traits() override;
      void setup_state(light::LightState *state) override { state_ = state; }
      void write_state(light::LightState *state) override;

      void set_min_value(uint32_t min_value) { min_value_ = min_value; }
      void set_max_value(uint32_t max_value) { max_value_ = max_value; }
    
    protected:
      bool set_by_touch_ = false;
      uint8_t min_value_ = 0;
      uint8_t max_value_ = 100;
      light::LightState *state_{nullptr};
      output::BinaryOutput *status_led_;
    
    private:
      void on_brightness_pct_message(const std::string &payload);
    };

  } // namespace gosund
} // namespace esphome
