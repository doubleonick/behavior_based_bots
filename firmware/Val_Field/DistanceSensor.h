/*
  DistanceSensor - the M5 ToF Unit (VL53L0X time-of-flight chip).

  Raw reading: distance in millimetres, about 30 to 1200 mm (2000 = nothing seen).
  Stimulus: nearer = more. Calibration makes this work: "low" is taken with
  nothing close (far), "high" with a hand close (near).
  Setup icons: a small square (far) and a big square (near).

  Needs the "VL53L0X" library by Pololu (Arduino Library Manager).
*/
#pragma once
#include <Arduino.h>
#include <VL53L0X.h>
#include "Sensor.h"

class DistanceSensor : public Sensor {
public:
  explicit DistanceSensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'D'; }
  const char* typeName() const override { return "Distance"; }
  float meterLevel() const override;

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 50; }
  long fullScale() const override { return MAX_MM; }
  long defaultLow() const override { return 1200; }   // far = no stimulus
  long defaultHigh() const override { return 30; }    // near = full stimulus

private:
  static const long MAX_MM = 2000;    // readings beyond this (or "nothing seen") count as this

  VL53L0X _chip;
  int _sda = -1;
  int _scl = -1;
};
