/*
  ReflectiveIRSensor - the M5 Reflective IR Unit.

  The unit shines infrared and measures how much bounces back: more when something
  pale or shiny is close. It is not an I2C sensor, so it cannot say what it is: a
  sketch only uses one where its settings say one is plugged in.

  The unit has two outputs: a level (analog) on its second wire, and on/off on its
  first (flips where the small knob on the unit is set). What Val can read depends on
  the port:
    Port A: the level, on G32 (0..4095). Calibrated in setup: "low" with nothing
            there, "high" with something close, so it works whichever way the
            voltage runs.
    Port B: on/off only. Port B is wired the other way round, so the on/off output
            lands on G33 and the level on G23, which cannot read a voltage.
            Raw 4095 while it sees something, 0 when it doesn't (the same scale as
            the level, so thresholds written for one still work); no calibration.
  Setup icons: the small and big squares, like the distance sensor.
*/
#pragma once
#include <Arduino.h>
#include "Sensor.h"

class ReflectiveIRSensor : public Sensor {
public:
  static const int LEVEL_PIN = 32;    // G32: the level, in Port A
  static const int ON_OFF_PIN = 33;   // G33: on/off, in Port B
  static const bool SEES_WHEN_LOW = true;   // on/off: the output goes LOW when it sees something

  explicit ReflectiveIRSensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'R'; }
  const char* typeName() const override { return "Reflective IR"; }
  float meterLevel() const override;
  bool needsCalibration() const override { return _level; }
  void reclaimPins();                 // after an I2C search used the port: get the pin back

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 20; }
  long fullScale() const override { return 4095; }
  long defaultLow() const override { return 0; }
  long defaultHigh() const override { return 4095; }

private:
  bool _level = false;                // true: reading the level (Port A); false: on/off (Port B)
  int  _pin = -1;
};
