/*
  DLightSensor - the M5 DLight Unit (U136, BH1750FVI): how bright it is, in lux.

  An I2C sensor, so Val finds it by itself in any port (and on a PaHub). The chip has
  no ID register; it is recognised by its address, 0x23 (or 0x5C on some boards),
  which no other unit Val uses answers at.

  Raw reading: lux, 0 to about 54 600. About 1 in a dark room, 300 in an ordinary
  room, 1000 under a desk lamp or a flashlight close up, 10 000+ outdoors.
  Stimulus: brighter = more, between the low and high points set in setup.
  Setup icons: moon (dark) and sun (bright), as for any light sensor.
*/
#pragma once
#include <Arduino.h>
#include "Sensor.h"

class DLightSensor : public Sensor {
public:
  explicit DLightSensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'L'; }
  const char* typeName() const override { return "DLight"; }
  float meterLevel() const override;

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 150; }   // a new measurement about every 120 ms
  long fullScale() const override { return 54612; }          // the most the chip can report, in lux
  long defaultLow() const override { return 0; }
  long defaultHigh() const override { return 1000; }

private:
  int _sda = -1;
  int _scl = -1;
  uint8_t _address = 0;

  void command(uint8_t c);
};
