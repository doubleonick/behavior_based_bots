#include "I2CBus.h"

namespace I2CBus {

namespace {
int currentSda = -1;
int currentScl = -1;
}

bool use(int sda, int scl, int channel) {
  if (sda != currentSda || scl != currentScl) {
    Wire1.end();
    Wire1.begin(sda, scl, 100000);
    delay(5);
    currentSda = sda;
    currentScl = scl;
  }
  if (channel < 0) {
    return true;                       // plugged straight in: no hub to talk to
  }
  // The hub connects one channel at a time: one bit per channel
  Wire1.beginTransmission(HUB_ADDRESS);
  Wire1.write(1 << channel);
  return Wire1.endTransmission() == 0;
}

bool hubAnswers() {
  return answers(HUB_ADDRESS);
}

void hubAllOff() {
  Wire1.beginTransmission(HUB_ADDRESS);
  Wire1.write(0);
  Wire1.endTransmission();
}

void release() {
  if (currentSda == -1) {
    return;                            // not using any pins
  }
  Wire1.end();
  currentSda = -1;
  currentScl = -1;
}

void releaseIfUsing(int pin) {
  if (pin == currentSda || pin == currentScl) {
    release();
  }
}

bool answers(uint8_t address) {
  Wire1.beginTransmission(address);
  return Wire1.endTransmission() == 0;
}

bool readByte(uint8_t address, uint8_t command, uint8_t& value) {
  Wire1.beginTransmission(address);
  Wire1.write(command);
  if (Wire1.endTransmission() != 0) {
    return false;
  }
  if (Wire1.requestFrom((int)address, 1) != 1) {
    return false;
  }
  value = Wire1.read();
  return true;
}

bool isDistanceChip() {
  uint8_t model = 0;
  uint8_t type = 0;
  if (!readByte(SHARED_ADDRESS, 0xC0, model)) {
    return false;
  }
  if (!readByte(SHARED_ADDRESS, 0xC1, type)) {
    return false;
  }
  return model == 0xEE && type == 0xAA;
}

bool isColorChip() {
  uint8_t id = 0;
  if (!readByte(SHARED_ADDRESS, 0x80 | 0x12, id)) {   // command bit + ID register
    return false;
  }
  return id == 0x44 || id == 0x4D || id == 0x10;
}

}
