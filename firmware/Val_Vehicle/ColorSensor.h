/*
  ColorSensor - the M5 Color Unit (TCS3472), used as a light sensor.

  Raw reading: the "clear" channel, overall brightness, 0 to 65535.
  Stimulus: brighter = more.  Setup icons: moon (dark) and sun (bright).
*/
#pragma once
#include <Arduino.h>
#include "Sensor.h"

class ColorSensor : public Sensor {
public:
  explicit ColorSensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'C'; }
  const char* typeName() const override { return "Light"; }
  float meterLevel() const override;

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 160; }   // one measurement every ~154 ms
  long fullScale() const override { return 65535; }
  long defaultLow() const override { return 0; }
  long defaultHigh() const override { return 65535; }

private:
  int _sda = -1;
  int _scl = -1;

  void writeRegister(uint8_t reg, uint8_t value);
};
