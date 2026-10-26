/*
  ReflectiveIRSensor - the M5 Reflective IR Unit, in Port B, read as on/off.

  The unit shines infrared and measures how much bounces back: more when something
  pale or shiny is close. It is not an I2C sensor, so it cannot say what it is. Val
  assumes one is in Port B when no I2C sensor answers there (if the sketch says to),
  and the setup screens let you switch it off.

  Port B is wired the other way round from Ports A and C, so the unit's on/off output
  lands on G33 and its analog output on G23, which cannot read a voltage. So in Port B
  the unit is only on/off: it "sees something" when the reflection passes the level
  set by the small knob (pot) on the unit. (For a level, the unit needs Port A; not
  supported yet.)

  Raw reading: 4095 while it sees something, 0 when it doesn't (the same scale as an
  analog reading, so thresholds written for one still work).
  Stimulus: 1 or 0, so setup skips calibration (needsCalibration() is false).
*/
#pragma once
#include <Arduino.h>
#include "Sensor.h"

class ReflectiveIRSensor : public Sensor {
public:
  static const int SIGNAL_PIN = 33;   // G33: the unit's on/off output, in Port B
  static const bool SEES_WHEN_LOW = true;   // the output goes LOW when it sees something

  explicit ReflectiveIRSensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'R'; }
  const char* typeName() const override { return "Reflective IR"; }
  float meterLevel() const override;
  bool needsCalibration() const override { return false; }
  void reclaimPins();                 // after an I2C search used the port: get the pin back

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 20; }
  long fullScale() const override { return 4095; }
  long defaultLow() const override { return 0; }
  long defaultHigh() const override { return 4095; }
};
