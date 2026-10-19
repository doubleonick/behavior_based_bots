#include "DistanceSensor.h"
#include "I2CBus.h"

// Look for a distance sensor on this port, trying both wire orders, and start it
bool DistanceSensor::begin() {
  const int orders[2][2] = {{_port.pin1, _port.pin2}, {_port.pin2, _port.pin1}};
  for (int n = 0; n < 2; n++) {
    I2CBus::use(orders[n][0], orders[n][1]);
    if (!I2CBus::answers(I2CBus::SHARED_ADDRESS)) {
      continue;
    }
    if (!I2CBus::isDistanceChip()) {
      continue;                               // something at 0x29, but not a distance sensor
    }
    _sda = orders[n][0];
    _scl = orders[n][1];
    _chip.setBus(&Wire1);
    _chip.setTimeout(100);
    if (!_chip.init()) {
      Serial.printf("Distance sensor on %s did not start\n", _port.name);
      continue;
    }
    _chip.startContinuous();                  // measure non-stop, ~30 times a second
    _connected = true;
    Serial.printf("Distance sensor found on %s (SDA=G%d, SCL=G%d)\n", _port.name, _sda, _scl);
    return true;
  }
  _connected = false;
  return false;
}

bool DistanceSensor::readRaw(long& out) {
  I2CBus::use(_sda, _scl);
  long mm = _chip.readRangeContinuousMillimeters();
  if (_chip.timeoutOccurred()) {
    return false;                             // no answer: probably unplugged
  }
  if (mm > MAX_MM) {
    mm = MAX_MM;                              // "nothing in range" comes back as a large number
  }
  out = mm;
  return true;
}

// Nearer = fuller meter
float DistanceSensor::meterLevel() const {
  float nearness = 1.0f - (float)raw() / defaultLow();
  return constrain(nearness, 0.0f, 1.0f);
}
