#include "PulseDrive.h"

void PulseDrive::setBeatsAtFull(float beats) {
  _beatsAtFull = constrain(beats, 0.2f, 5.0f);
}

const char* PulseDrive::shapeName(Shape s) {
  if (s == PUSH_GLIDE) {
    return "push and glide";
  }
  return "on and off";
}

// A beat's strength through the beat (share 0 = its start, 1 = its end).
float PulseDrive::shapeAt(float share) const {
  if (_shape == ON_OFF) {
    return 1.0f;
  }
  // push and glide: a quick rise over the first 15%, then easing off
  if (share < 0.15f) {
    return share / 0.15f;
  }
  return expf(-(share - 0.15f) * 3.2f);
}

void PulseDrive::step(float strongest) {
  uint32_t now = millis();
  if (!_started) {
    _started = true;
    _lastMs = now;
  }
  float seconds = (now - _lastMs) / 1000.0f;
  _lastMs = now;

  if (!_enabled) {
    _gain = 1;                        // the regular drive: hand every amount back as it is
    return;
  }

  strongest = constrain(fabsf(strongest), 0.0f, 1.0f);
  float beatLength = 1.0f / _beatsAtFull;

  // The beat clock: the stronger the wiring, the faster it runs. Each time it comes
  // round, a new beat starts (even if the last one is still easing off).
  _phase += strongest * _beatsAtFull * seconds;
  if (_phase >= 1.0f) {
    _phase -= 1.0f;
    if (_phase >= 1.0f) {
      _phase = 0;
    }
    _beatAge = 0;
  } else if (_beatAge >= 0) {
    _beatAge += seconds;
    if (_beatAge >= beatLength) {
      _beatAge = -1;                  // the beat is over: wait for the next
    }
  }

  // During a beat the strongest wheel pushes at full (times the beat's shape); the
  // others in proportion. Between beats, nothing.
  if (_beatAge >= 0 && strongest > 0.001f) {
    _gain = shapeAt(_beatAge / beatLength) / strongest;
  } else {
    _gain = 0;
  }
}

float PulseDrive::apply(float amount) const {
  return constrain(amount * _gain, -1.0f, 1.0f);
}
