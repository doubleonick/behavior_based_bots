#include "LightUnitSensor.h"
#include "I2CBus.h"

// Only Port A has the pin. Nothing to ask the unit, so this just gets the pin ready.
bool LightUnitSensor::begin() {
  if (_port.pin1 != LEVEL_PIN && _port.pin2 != LEVEL_PIN) {
    _connected = false;
    return false;                             // not Port A
  }
  reclaimPins();
  _connected = true;
  Serial.printf("Light Unit assumed on %s (level on G%d)\n", _port.name, LEVEL_PIN);
  return true;
}

void LightUnitSensor::reclaimPins() {
  I2CBus::releaseIfUsing(LEVEL_PIN);          // the I2C search may have been using it
  pinMode(LEVEL_PIN, INPUT);
}

bool LightUnitSensor::readRaw(long& out) {
  I2CBus::releaseIfUsing(LEVEL_PIN);
  out = analogRead(LEVEL_PIN);
  return true;                                // an analog pin always gives a number
}

float LightUnitSensor::meterLevel() const {
  return constrain((float)raw() / fullScale(), 0.0f, 1.0f);   // the raw reading, so the
                                                              // meter moves before calibration
}
