#include "MotionServo.h"

namespace {
const uint8_t  MOTION_ADDRESS = 0x38;
const float    SMOOTHING      = 0.15f;   // 0..1, lower = smoother, slower to follow
const uint32_t UPDATE_MS      = 20;
}

MotionServo::MotionServo(uint8_t channel) : _channel(channel) {
}

// The Motion base is on the Atom's internal pins G25 and G21. Try both orders.
bool MotionServo::begin() {
  const int pins[2][2] = {{25, 21}, {21, 25}};
  for (int n = 0; n < 2; n++) {
    Wire.end();
    Wire.begin(pins[n][0], pins[n][1], 100000);
    delay(5);
    Wire.beginTransmission(MOTION_ADDRESS);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Atom Motion found (SDA=G%d, SCL=G%d)\n", pins[n][0], pins[n][1]);
      stopNow();
      return true;
    }
  }
  Wire.end();
  Wire.begin(25, 21, 100000);
  Serial.println("Atom Motion NOT found - is the Atom seated and the Motion switched on?");
  return false;
}

void MotionServo::setMount(Mount m) {
  _mount = m;
}

const char* MotionServo::mountName(Mount m) {
  if (m == LEFT) {
    return "left";
  }
  return "right";
}

void MotionServo::setAllowReverse(bool allow) {
  _allowReverse = allow;
}

void MotionServo::setStopAngle(int degrees) {
  _stopAngle = constrain(degrees, 0, 180);
}

void MotionServo::drive(float amount) {
  float lowest = 0;
  if (_allowReverse) {
    lowest = -1;
  }
  _target = constrain(amount, lowest, 1.0f);
}

void MotionServo::stopNow() {
  _target = 0;
  _smooth = 0;
  sendAngle(_stopAngle);
}

void MotionServo::update() {
  if (millis() - _lastUpdateMs < UPDATE_MS) {
    return;
  }
  _lastUpdateMs = millis();
  _smooth += SMOOTHING * (_target - _smooth);
  sendAngle(angleFor(_smooth));
}

// drive -1..1 -> angle. Left mount: forward is toward 180. Right mount: toward 0.
int MotionServo::angleFor(float amount) const {
  float direction = 1;
  if (_mount == RIGHT) {
    direction = -1;
  }
  int degrees = (int)lroundf(_stopAngle + direction * amount * 90);
  return constrain(degrees, 0, 180);
}

void MotionServo::sendAngle(int degrees) {
  if (degrees == _lastAngle) {
    return;                               // only send when it changes
  }
  Wire.beginTransmission(MOTION_ADDRESS);
  Wire.write(_channel - 1);               // register 0x00-0x03 = S1-S4 angle
  Wire.write((uint8_t)degrees);
  Wire.endTransmission();
  _lastAngle = degrees;
}
