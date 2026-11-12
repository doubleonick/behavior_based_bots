/*
  Neuron - one nerve cell, the part Vehicles 4, 5 and 7 are built from.

  A neuron adds up what its inputs tell it. Below its threshold it stays silent.
  Above the threshold it fires harder the more it is driven, up to a ceiling (1).
  It does not change instantly: it moves towards its new firing rate over a short time.

      drive  = (what comes in) + selfWeight x (what it fired a moment ago)
      target = gain x (drive - threshold),  kept between 0 and 1
      output moves a step towards target

  The settings are things evolution could tune, one small change at a time:

    threshold   how much it takes to make the neuron fire at all
    gain        1  = graded: it fires in proportion to how far above the threshold
                20 = all-or-none: just above the threshold it is already at full rate
    timeMs      how quickly it follows its target (about this many milliseconds)
    selfWeight  0  = it only listens to its inputs (Vehicle 4)
                more = it also listens to itself. Enough of that, and once it is on
                it keeps itself on after the input has gone: memory (Vehicle 5).
                It stays on if selfWeight is at least threshold + 1 / gain.

  The output is a firing RATE (0 = silent, 1 = as fast as it can), not single spikes.
*/
#pragma once
#include <Arduino.h>

class Neuron {
public:
  float threshold  = 0.5f;
  float gain       = 1.0f;
  float timeMs     = 100.0f;
  float selfWeight = 0.0f;

  // Give the neuron its input (the sum of what is wired into it) and let a little
  // time pass. Returns how hard it is firing now, 0..1.
  float update(float input) {
    unsigned long now = millis();
    float elapsedMs = (float)(now - _lastMs);
    _lastMs = now;

    float drive = input + selfWeight * _output;

    float target = gain * (drive - threshold);
    if (target < 0) {
      target = 0;                      // silent below the threshold
    }
    if (target > 1) {
      target = 1;                      // it cannot fire harder than its ceiling
    }

    // Move part of the way from where it is to the target. A long pause (or the very
    // first call) counts as "enough time to get all the way there".
    float part = 1;
    if (timeMs > 0 && elapsedMs < timeMs) {
      part = elapsedMs / timeMs;
    }
    _output = _output + part * (target - _output);
    return _output;
  }

  // How hard it was firing the last time update() ran
  float output() const {
    return _output;
  }

  // Silence it (for example when setup starts again)
  void reset() {
    _output = 0;
  }

private:
  float _output = 0;
  unsigned long _lastMs = 0;
};
