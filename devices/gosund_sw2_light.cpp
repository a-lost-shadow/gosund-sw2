#include "gosund_sw2_light.hpp"

namespace esphome {
  namespace gosund {
    static const char *TAG = "gosund.light.sw2";
    static const byte ON_MASK = 0x80;

    static const uint8_t MIN_VALUE = 0;
    static const uint8_t MAX_VALUE = 100;
    static const uint8_t MIN_PERCENT = 1;

    void GosundLight::populateBrightnessTopic(char * topic, int len) {
      snprintf(topic, len, "%s/light/%s/command/brightness_pct",
	       mqtt::global_mqtt_client->get_topic_prefix().c_str(),
	       state_->get_object_id().c_str());
    }      

    void GosundLight::setup() {
      char topic[256];
      populateBrightnessTopic(topic, sizeof(topic));
      subscribe(topic, &GosundLight::on_brightness_pct_message);
      
      ESP_LOGD(TAG, "Subscribed to: %s", topic);
    }

    void GosundLight::loop() {
      // pattern is 0x24 0xYY 0x01 0x1E 0x23, where YY is dimmer value
      if (Serial.available() >= 5) {
	char buff[5];

	//
	while (Serial.available() >= 5) {
	  Serial.readBytes(buff, 5);
	}
	if (Serial.available() > 0) {
	  ESP_LOGD(TAG, "Unexpected bytes on serial. ");
	  ESP_LOGD(TAG, "  Expected: %02X %02X %02X %02X %02X",
		   buff[0], buff[1], buff[2], buff[3], buff[4]);
	  
	  while (Serial.available() >= 1) {
	    Serial.readBytes(buff, 1);
	    ESP_LOGD(TAG, "  Unexpected: %02X", buff[0]);
	  }
	}

	float dimmerVal = .01 * buff[1];

	set_by_touch_ = true;    /* Prevent sending the value back to the hardware */
	auto call = state_->make_call();
	// Touch sensor only works when turned on
	call.set_state(true);
	call.set_brightness(dimmerVal);
	call.perform();
      
	ESP_LOGD(TAG, "Received dimmer value %02X %d", buff[1], buff[1]);
      }
    }
  
    void GosundLight::dump_config() {
      char topic[256];
      populateBrightnessTopic(topic, sizeof(topic));

      ESP_LOGCONFIG(TAG, "Gosund SW2 Dimmer:");
      ESP_LOGCONFIG(TAG, " Brightness Percent Topic: %s", topic);
    }
  
    light::LightTraits GosundLight::get_traits() {
      auto traits = light::LightTraits();
      traits.set_supports_brightness(true);
      return traits;    
    }
  
    void GosundLight::write_state(light::LightState *state) {
      // don't go into an infinite loop when set by touch
      if (set_by_touch_) {
	set_by_touch_ = false;
	return;
      }
    
      auto values = state->current_values;
      uint8_t output = std::min(MAX_VALUE,
				(uint8_t) (100 * values.get_brightness()));
      output = std::max(MIN_VALUE, output);
    
      if (values.get_state() > 0 && values.get_brightness() > 0) {
	output += ON_MASK;
	status_led_->turn_on();
	ESP_LOGD(TAG, "turning on status LED");
      } else {
	status_led_->turn_off();
	ESP_LOGD(TAG, "turning off status LED");
      }

      ESP_LOGD(TAG, "write_state() called with state: %0.1f, brightness: %.02f => output: %02X",
	       values.get_state(), values.get_brightness(), output);

      Serial.write(output);
    }  

    void GosundLight::on_brightness_pct_message(const std::string &payload) {
      uint8_t brightness = atoi(payload.c_str());

      ESP_LOGD(TAG, "Received MQTT Brightness request for %d", brightness);
      
      brightness = std::max(MIN_PERCENT, brightness);
      brightness = std::min(MAX_VALUE, brightness);

      float brightnessFloat = 0.01 * brightness;

      auto call = state_->make_call();
      call.set_brightness(brightnessFloat);
      set_by_touch_ = false;
      call.perform();
    }
  } // namespace gosund
} // namespace esphome
