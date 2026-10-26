#include "ReflectiveIRSensor.h"
#include "I2CBus.h"

// Only Port B is supported. Nothing to ask the unit, so this just gets the pin ready.
bool ReflectiveIRSensor::begin() {
  if (_port.pin1 != SIGNAL_PIN && _port.pin2 != SIGNAL_PIN) {
    _connected = false;
    return false;                             // not Port B
  }
  reclaimPins();
  _connected = true;
  Serial.printf("Reflective IR assumed on %s (on/off on G%d)\n", _port.name, SIGNAL_PIN);
  return true;
}

void ReflectiveIRSensor::reclaimPins() {
  I2CBus::releaseIfUsing(SIGNAL_PIN);         // the I2C search may have been using it
  pinMode(SIGNAL_PIN, INPUT);
}

bool ReflectiveIRSensor::readRaw(long& out) {
  I2CBus::releaseIfUsing(SIGNAL_PIN);
  bool low = (digitalRead(SIGNAL_PIN) == LOW);
  bool sees = low;
  if (!SEES_WHEN_LOW) {
    sees = !low;
  }
  if (sees) {
    out = 4095;
  } else {
    out = 0;
  }
  return true;                                // a pin always gives a reading
}

float ReflectiveIRSensor::meterLevel() const {
  return constrain((float)raw() / fullScale(), 0.0f, 1.0f);
}
