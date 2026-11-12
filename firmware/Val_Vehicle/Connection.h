/*
  Connection - one wire from a sensor to the motor, in Braitenberg's sense.
  (It is called Connection because Arduino already uses the name "Wire" for I2C.)

  weight:  +1 = excitatory   (more stimulus -> faster)
           -1 = inhibitory   (more stimulus -> slower)
            0 = not connected

  curve:   the shape of the wire. What the sensor feels (0..1) goes in, and what the
           wire passes on (0..1) comes out. This is where Vehicle 4 lives.

             STRAIGHT  out = in. Vehicles 1 to 3.

             HUMP      Vehicle 4a: the wire has a favourite level. It passes most at
                       that level, and less and less on either side of it:

                         out 1 |      /\
                               |     /  \
                             0 |____/    \____
                                    level        in
                                  <-width->

             STEP      Vehicle 4b: a threshold. Nothing passes until the sensor
                       feels at least `level`; from then on everything does.

                         out 1 |       ______
                               |      |
                             0 |______|
                                    level        in

  level:   HUMP: the favourite level. STEP: the threshold.
           0 = the sensor's low point, 1 = its high point (the ones set in setup).
  width:   HUMP only: how far from the favourite level the wire still passes anything.
*/
#pragma once
#include <math.h>
#include "Sensor.h"

class Connection {
public:
  enum Curve : uint8_t { STRAIGHT, HUMP, STEP };

  Sensor* sensor = nullptr;
  float   weight = 1.0f;
  Curve   curve  = STRAIGHT;
  float   level  = 0.5f;
  float   width  = 0.35f;

  // A real sensor's reading wobbles. Without a gap, a STEP wire sitting right at its
  // threshold would flick on and off. So once it is on, it stays on until the
  // reading has dropped this far below the threshold.
  static constexpr float STEP_GAP = 0.05f;

  bool isActive() const {
    return sensor != nullptr && sensor->isConnected() && weight != 0;
  }

  // The shape of the wire: x is what the sensor feels (0..1)
  float transfer(float x) {
    if (curve == HUMP) {
      if (width <= 0) {
        return 0;
      }
      float away = fabsf(x - level) / width;   // 0 at the favourite level, 1 at the edge
      if (away >= 1) {
        return 0;
      }
      return 1 - away;
    }
    if (curve == STEP) {
      if (x >= level) {
        _stepOn = true;
      } else if (x < level - STEP_GAP) {
        _stepOn = false;
      }
      if (_stepOn) {
        return 1;
      }
      return 0;
    }
    return x;                                  // STRAIGHT
  }

  // What this wire delivers to the motor
  float output() {
    if (!isActive()) {
      return 0;
    }
    _passed = transfer(sensor->value());
    return weight * _passed;
  }

  // What the curve passed on the last time output() ran (for printing)
  float passed() const {
    return _passed;
  }

  static const char* curveName(Curve c) {
    if (c == HUMP) {
      return "hump";
    }
    if (c == STEP) {
      return "step";
    }
    return "straight";
  }

private:
  bool  _stepOn = false;
  float _passed = 0;
};
