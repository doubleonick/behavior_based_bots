#include "Sensor.h"

namespace {
const float CHANGE_FRACTION = 0.15f;   // a 15% change counts as activity
const float DRIFT_RATE      = 0.02f;   // how quickly the reference follows slow changes
}

bool Sensor::update() {
  if (!_connected) {
    return false;
  }
  if (millis() - _lastReadMs < readIntervalMs()) {
    return false;
  }
  _lastReadMs = millis();

  long reading = 0;
  if (!readRaw(reading)) {
    _connected = false;
    Serial.printf("%s sensor on %s stopped answering\n", typeName(), portName());
    return false;
  }
  _raw = reading;

  // Did the reading change noticeably? Slow drift (daylight fading) does not count.
  if (!_haveReference) {
    _reference = _raw;
    _haveReference = true;
  } else {
    float floorLevel = fullScale() * 0.0015f;           // ignore tiny changes near zero
    float base = max(_reference, floorLevel);
    if (fabsf(_raw - _reference) / base > CHANGE_FRACTION) {
      _reference = _raw;
      _changed = true;
    } else {
      _reference += DRIFT_RATE * (_raw - _reference);
    }
  }
  return true;
}

long Sensor::lowPoint() const {
  if (_low == NOT_SET) {
    return defaultLow();
  }
  return _low;
}

long Sensor::highPoint() const {
  if (_high == NOT_SET) {
    return defaultHigh();
  }
  return _high;
}

// Straight line from the low point (0) to the high point (1), kept within 0..1
float Sensor::value() const {
  float span = (float)(highPoint() - lowPoint());
  if (fabsf(span) < 1) {
    return 0;
  }
  float v = (_raw - lowPoint()) / span;
  if (v <= 0) {
    return 0;                          // (also avoids printing "-0.00")
  }
  return min(v, 1.0f);
}

void Sensor::setCalibration(long low, long high) {
  _low = low;
  _high = high;
}

void Sensor::clearCalibration() {
  _low = NOT_SET;
  _high = NOT_SET;
}

bool Sensor::takeChange() {
  bool changed = _changed;
  _changed = false;
  return changed;
}
