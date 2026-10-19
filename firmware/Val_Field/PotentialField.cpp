#include "PotentialField.h"
#include <math.h>

void PotentialField::compute() {
  float vx = 0;
  float vy = 0;

  for (int i = 0; i < MAX_SOURCES; i++) {
    const Source& s = source[i];
    if (!s.enabled || s.sensor == nullptr || s.policy == OFF) {
      continue;
    }

    float length = s.sensor->value() * s.gain;       // 0..1 times gain

    // Pull points toward where the sensor looks; push points the other way
    float direction = s.angle;
    if (s.policy == PUSH) {
      direction += 180;
    }
    float radians = direction * PI / 180.0f;

    // Unit vector for an angle measured counter-clockwise from forward:
    //   forward (+Y) part = cos(angle)
    //   right   (+X) part = -sin(angle)   (turning counter-clockwise leans left)
    vx += length * -sinf(radians);
    vy += length * cosf(radians);
  }

  _vx = vx;
  _vy = vy;
}

void PotentialField::toWheels(float& left, float& right) const {
  float forward = baseSpeed + forwardGain * _vy;
  float turn = turnGain * _vx;
  left = constrain(forward + turn, -1.0f, 1.0f);
  right = constrain(forward - turn, -1.0f, 1.0f);
}

const char* PotentialField::policyName(Policy p) {
  if (p == PULL) {
    return "pull";
  }
  if (p == PUSH) {
    return "push";
  }
  return "off";
}
