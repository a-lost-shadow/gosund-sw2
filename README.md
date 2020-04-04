Gosund SW2 Implementation for ESP Home
=======================

Features
--------
- Reading of the touch sensor
- Integration via both ESP Home Native API & MQTT
- Additional MQTT topic for setting the brightness without changing the state of the light
- White LED turns on when the light is on
- Green LED flashes when first powered until wifi connects


To use
------
- Update devices/secrets.yaml
- Update example_sw2.yaml with the id and friendly name for your switch.  The id will be used as the hostname and id in Home Assistant.
- Rename example_sw2.yaml
- Consider disabling the web interface in devices/gosund_sw2_light.yaml
- Compile with esphome
- Flash

MQTT
----
The MQTT implementation uses the ESP Home's pattern for auto discovery.  The topics for status and control of the light will be under
`{app}/light/{id}`.  Where `{app}` is the name of the yaml file without the extension, and `{id}` is the device_id inside the yaml file.

StateTopic: `{app}/light/{id}/state`
* Payload is json formatted: `{"state":"ON","brightness":255}`

Command Topic: `{app}/light/{id}/command`
* Same JSON formatting as the state topic
* state is required and is either ON or OFF
* brightness is optional with a range of 0-255

Brightness Command Topic: `{app}/light/id/command/brightness_pct`
* Changes the brightness without changing the state of the light
* Payload is an integer in the range of 1-100
