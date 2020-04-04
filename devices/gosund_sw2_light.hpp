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

    protected:
      bool set_by_touch_ = false;
      light::LightState *state_{nullptr};
      output::BinaryOutput *status_led_;
    
    private:
      void on_brightness_pct_message(const std::string &payload);
      void populateBrightnessTopic(char * topic, int len);
    };

  } // namespace gosund
} // namespace esphome
