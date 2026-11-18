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

// Where each channel sits on the colour wheel (LIGHT has none), and how far counts
float ColorSensor::s_hue[ColorSensor::CHANNEL_COUNT] = {0, 300, 60, 180};
float ColorSensor::s_width = 90;

void ColorSensor::setHues(float magenta, float yellow, float cyan, float width) {
  s_hue[MAGENTA] = magenta;
  s_hue[YELLOW] = yellow;
  s_hue[CYAN] = cyan;
  s_width = width;
}

const char* ColorSensor::channelName(Channel c) {
  if (c == MAGENTA) {
    return "Magenta";
  }
  if (c == YELLOW) {
    return "Yellow";
  }
  if (c == CYAN) {
    return "Cyan";
  }
  return "Light";
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
  long clear = d[0] | (d[1] << 8);            // low byte first
  _red   = d[2] | (d[3] << 8);
  _green = d[4] | (d[5] << 8);
  _blue  = d[6] | (d[7] << 8);
  if (_channel == LIGHT) {
    out = clear;                              // how bright, any colour
  } else {
    out = max(_red, max(_green, _blue));      // how bright the colour is
  }
  return true;
}

// How much what it sees is like this channel's colour, 0..1
float ColorSensor::colourMatch() const {
  if (_channel == LIGHT) {
    return 1;
  }
  float r = (float)_red;
  float g = (float)_green;
  float b = (float)_blue;
  float biggest = max(r, max(g, b));
  float smallest = min(r, min(g, b));
  if (biggest <= 0) {
    return 0;                                 // no light at all: no colour
  }

  // Hue: where the colour sits round the wheel, in degrees
  float spread = biggest - smallest;
  float hue = 0;
  if (spread > 0) {
    if (biggest == r) {
      hue = 60 * ((g - b) / spread);
    } else if (biggest == g) {
      hue = 60 * ((b - r) / spread + 2);
    } else {
      hue = 60 * ((r - g) / spread + 4);
    }
  }
  if (hue < 0) {
    hue += 360;
  }

  // How close that is to the channel's own hue (the short way round the wheel)
  float away = fabsf(hue - s_hue[_channel]);
  if (away > 180) {
    away = 360 - away;
  }
  float closeness = 0;
  if (s_width > 0 && away < s_width) {
    closeness = 1 - away / s_width;
  }

  // How much colour there is at all: 0 for white or grey light
  float saturation = spread / biggest;
  return closeness * saturation;
}

// The stimulus: brightness between the calibration points, times the colour match
float ColorSensor::value() const {
  return Sensor::value() * colourMatch();
}

// Square-root scaled so dim indoor light still shows on the meter
float ColorSensor::meterLevel() const {
  float fraction = (float)raw() / fullScale();
  return sqrtf(constrain(fraction, 0.0f, 1.0f));
}
