/*
  ReflectiveIRSensor - the M5 Reflective IR Unit, read as an analog value.

  The unit shines infrared and measures how much bounces back: more when something
  pale or shiny is close. It is not an I2C sensor, so it cannot say what it is. Val
  assumes one is in Port B when no I2C sensor answers there, and the setup screens
  let you check it (watch the meter) or switch it off.

  Raw reading: 0 to 4095 from the ESP32's ADC on G33 (Port B's only analog pin), so
  the unit's analog wire must be on G33.
  Stimulus: calibration decides which way is "more" (low = nothing there, high =
  something close), so it works whichever way the unit's voltage runs.
  Setup icons: the small and big squares, like the distance sensor.
*/
#pragma once
#include <Arduino.h>
#include "Sensor.h"

class ReflectiveIRSensor : public Sensor {
public:
  static const int ANALOG_PIN = 33;   // G33, Port B

  explicit ReflectiveIRSensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'R'; }
  const char* typeName() const override { return "Reflective IR"; }
  float meterLevel() const override;
  void reclaimPins();                 // after an I2C search used the port: get the pin back

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 20; }
  long fullScale() const override { return 4095; }
  long defaultLow() const override { return 0; }
  long defaultHigh() const override { return 4095; }
};
