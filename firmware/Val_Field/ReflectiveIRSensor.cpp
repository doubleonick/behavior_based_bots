#include "ReflectiveIRSensor.h"
#include "I2CBus.h"

// Only Port B has the analog pin. Nothing to ask the unit, so this just gets the pin ready.
bool ReflectiveIRSensor::begin() {
  if (_port.pin1 != ANALOG_PIN && _port.pin2 != ANALOG_PIN) {
    _connected = false;
    return false;                             // not Port B: no analog pin
  }
  I2CBus::releaseIfUsing(ANALOG_PIN);         // the I2C search may have been using it
  pinMode(ANALOG_PIN, INPUT);
  _connected = true;
  Serial.printf("Reflective IR assumed on %s (analog on G%d)\n", _port.name, ANALOG_PIN);
  return true;
}

void ReflectiveIRSensor::reclaimPins() {
  I2CBus::releaseIfUsing(ANALOG_PIN);
  pinMode(ANALOG_PIN, INPUT);
}

bool ReflectiveIRSensor::readRaw(long& out) {
  I2CBus::releaseIfUsing(ANALOG_PIN);
  out = analogRead(ANALOG_PIN);
  return true;                                // an analog pin always gives a number
}

float ReflectiveIRSensor::meterLevel() const {
  return constrain((float)raw() / fullScale(), 0.0f, 1.0f);
}
