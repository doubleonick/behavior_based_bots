#include "ColorSensor.h"
#include "I2CBus.h"

namespace {
const uint8_t ADDRESS     = I2CBus::SHARED_ADDRESS;
const uint8_t CMD         = 0x80;   // command bit
const uint8_t AUTO_INC    = 0x20;   // read several registers in a row
const uint8_t REG_ENABLE  = 0x00;
const uint8_t REG_ATIME   = 0x01;
const uint8_t REG_CONTROL = 0x0F;
const uint8_t REG_CDATAL  = 0x14;   // clear, red, green, blue: 2 bytes each

const uint8_t ATIME_VALUE = 0xC0;   // 64 steps x 2.4 ms = ~154 ms per measurement
const uint8_t GAIN_VALUE  = 1;      // 0 = 1x, 1 = 4x, 2 = 16x, 3 = 60x
}

void ColorSensor::writeRegister(uint8_t reg, uint8_t value) {
  Wire1.beginTransmission(ADDRESS);
  Wire1.write(CMD | reg);
  Wire1.write(value);
  Wire1.endTransmission();
}

// Look for a color sensor on this port, trying both wire orders, and switch it on
bool ColorSensor::begin() {
  const int orders[2][2] = {{_port.pin1, _port.pin2}, {_port.pin2, _port.pin1}};
  for (int n = 0; n < 2; n++) {
    if (!I2CBus::use(orders[n][0], orders[n][1], _port.channel)) {
      continue;                               // should be on a PaHub, and no hub answered
    }
    if (!I2CBus::answers(ADDRESS)) {
      continue;
    }
    if (I2CBus::isDistanceChip()) {
      continue;                               // something at 0x29, but it's a distance sensor
    }
    if (!I2CBus::isColorChip()) {
      continue;
    }
    _sda = orders[n][0];
    _scl = orders[n][1];
    writeRegister(REG_ATIME, ATIME_VALUE);
    writeRegister(REG_CONTROL, GAIN_VALUE);
    writeRegister(REG_ENABLE, 0x01);          // power on
    delay(3);
    writeRegister(REG_ENABLE, 0x03);          // start measuring
    _connected = true;
    Serial.printf("Light sensor found on %s (SDA=G%d, SCL=G%d)\n", _port.name, _sda, _scl);
    return true;
  }
  _connected = false;
  return false;
}

// Read all four channels in one go (as the earlier sketches did) and keep "clear"
bool ColorSensor::readRaw(long& out) {
  if (!I2CBus::use(_sda, _scl, _port.channel)) {
    return false;                             // the PaHub stopped answering
  }
  Wire1.beginTransmission(ADDRESS);
  Wire1.write(CMD | AUTO_INC | REG_CDATAL);
  if (Wire1.endTransmission() != 0) {
    return false;
  }
  if (Wire1.requestFrom((int)ADDRESS, 8) != 8) {
    return false;
  }
  uint8_t d[8];
  for (int i = 0; i < 8; i++) {
    d[i] = Wire1.read();
  }
  out = d[0] | (d[1] << 8);                   // clear channel
  return true;
}

// Square-root scaled so dim indoor light still shows on the meter
float ColorSensor::meterLevel() const {
  float fraction = (float)raw() / fullScale();
  return sqrtf(constrain(fraction, 0.0f, 1.0f));
}
