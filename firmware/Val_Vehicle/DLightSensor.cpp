#include "DLightSensor.h"
#include "I2CBus.h"
#include <math.h>

namespace {
const uint8_t ADDRESS_LOW  = 0x23;      // the usual address (the chip's ADDR pin low)
const uint8_t ADDRESS_HIGH = 0x5C;      // the other one (ADDR pin high)
const uint8_t POWER_ON = 0x01;
const uint8_t MEASURE_AGAIN_AND_AGAIN = 0x10;   // continuous, 1 lux steps, ~120 ms each
}

void DLightSensor::command(uint8_t c) {
  Wire1.beginTransmission(_address);
  Wire1.write(c);
  Wire1.endTransmission();
}

// Look for a DLight on this place, trying both wire orders and both addresses
bool DLightSensor::begin() {
  const int orders[2][2] = {{_port.pin1, _port.pin2}, {_port.pin2, _port.pin1}};
  const uint8_t addresses[2] = {ADDRESS_LOW, ADDRESS_HIGH};
  for (int n = 0; n < 2; n++) {
    if (!I2CBus::use(orders[n][0], orders[n][1], _port.channel)) {
      continue;                               // should be on a PaHub, and no hub answered
    }
    for (int a = 0; a < 2; a++) {
      if (!I2CBus::answers(addresses[a])) {
        continue;
      }
      _sda = orders[n][0];
      _scl = orders[n][1];
      _address = addresses[a];
      command(POWER_ON);
      command(MEASURE_AGAIN_AND_AGAIN);
      _connected = true;
      Serial.printf("DLight found on %s (SDA=G%d, SCL=G%d, address 0x%02X)\n",
                    _port.name, _sda, _scl, _address);
      return true;
    }
  }
  _connected = false;
  return false;
}

// Two bytes, high byte first; divided by 1.2 that is lux
bool DLightSensor::readRaw(long& out) {
  if (!I2CBus::use(_sda, _scl, _port.channel)) {
    return false;                             // the PaHub stopped answering
  }
  if (Wire1.requestFrom((int)_address, 2) != 2) {
    return false;
  }
  uint16_t counts = (Wire1.read() << 8) | Wire1.read();
  out = (long)(counts / 1.2f + 0.5f);
  return true;
}

// Logarithmic, so a dark room, an ordinary room and a flashlight all show
float DLightSensor::meterLevel() const {
  float fraction = log10f(1 + (float)raw()) / log10f(1 + (float)fullScale());
  return constrain(fraction, 0.0f, 1.0f);
}
