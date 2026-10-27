/*
  LightUnitSensor - the M5 Light Unit (U021): a light-dependent resistor.

  Not an I2C sensor, so it cannot say what it is: a sketch only uses one where its
  settings say one is plugged in.

  Like the Reflective IR Unit, it has a level (analog) on its second wire and on/off
  on its first. Val reads the level, so it only works in Port A (G32). (In Port B the
  level lands on G23, which cannot read a voltage; Port C has no analog pins.)

  Raw reading: 0 to 4095. Calibrated in setup: "low" covered (dark), "high" lit, so it
  works whichever way the voltage runs.
  Setup icons: the light sensor's (dark and bright), like the Color Unit.
*/
#pragma once
#include <Arduino.h>
#include "Sensor.h"

class LightUnitSensor : public Sensor {
public:
  static const int LEVEL_PIN = 32;    // G32, Port A

  explicit LightUnitSensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'U'; }
  const char* typeName() const override { return "Light Unit"; }
  float meterLevel() const override;
  void reclaimPins();                 // after an I2C search used the port: get the pin back

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 20; }
  long fullScale() const override { return 4095; }
  long defaultLow() const override { return 0; }
  long defaultHigh() const override { return 4095; }
};
