#include "ReflectiveIRSensor.h"
#include "I2CBus.h"

// Port A: the level. Port B: on/off. Nothing to ask the unit, so this just gets the pin ready.
bool ReflectiveIRSensor::begin() {
  if (_port.pin1 == LEVEL_PIN || _port.pin2 == LEVEL_PIN) {
    _level = true;
    _pin = LEVEL_PIN;
  } else if (_port.pin1 == ON_OFF_PIN || _port.pin2 == ON_OFF_PIN) {
    _level = false;
    _pin = ON_OFF_PIN;
  } else {
    _connected = false;
    return false;                             // Port C: nothing Val can use
  }
  reclaimPins();
  _connected = true;
  if (_level) {
    Serial.printf("Reflective IR assumed on %s (level on G%d)\n", _port.name, _pin);
  } else {
    Serial.printf("Reflective IR assumed on %s (on/off on G%d)\n", _port.name, _pin);
  }
  return true;
}

void ReflectiveIRSensor::reclaimPins() {
  if (_pin < 0) {
    return;
  }
  I2CBus::releaseIfUsing(_pin);               // the I2C search may have been using it
  pinMode(_pin, INPUT);
}

bool ReflectiveIRSensor::readRaw(long& out) {
  I2CBus::releaseIfUsing(_pin);
  if (_level) {
    out = analogRead(_pin);
    return true;                              // an analog pin always gives a number
  }
  bool low = (digitalRead(_pin) == LOW);
  bool sees = low;
  if (!SEES_WHEN_LOW) {
    sees = !low;
  }
  if (sees) {
    out = 4095;
  } else {
    out = 0;
  }
  return true;
}

float ReflectiveIRSensor::meterLevel() const {
  return constrain((float)raw() / fullScale(), 0.0f, 1.0f);   // the raw reading, so the
                                                              // meter moves before calibration
}
