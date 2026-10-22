/*
  PotentialField - steering by adding up pushes and pulls (Val version of
  PAW-Robotics' CogPotentialField).

  Each sensor reading IS the field measured in the direction the sensor points.
  For every sensor we build a vector:
      length    = strength (0..1, from the sensor's calibration) x gain
      direction = the sensor's mount angle            for PULL (attract)
                = the mount angle + 180 degrees        for PUSH (repel)
  All the vectors are added into one net vector, and that is turned into
  left/right wheel speeds. Steering comes out of where the sensors point;
  nothing is special-cased.

  Directions use the same frame as PAW:
      +Y = forward, +X = right
      angles in degrees counter-clockwise from forward:
         0 = straight ahead, +90 = left, -90 = right, 180 = behind

  Wheels (same mix as PAW's vectorToDifferential):
      forward = baseSpeed + forwardGain x Vy
      turn    = turnGain x Vx
      left    = forward + turn       right = forward - turn       (each -1..1)
  A vector pointing right (+Vx) speeds up the left wheel, so the robot turns right.
*/
#pragma once
#include <Arduino.h>
#include "Sensor.h"

class PotentialField {
public:
  enum Policy : uint8_t { PULL, PUSH, OFF };

  struct Source {
    Sensor* sensor = nullptr;
    bool    enabled = false;          // set each loop: plugged in and set up
    Policy  policy = PULL;
    float   angle = 0;                // mount angle, degrees CCW from forward
    float   gain = 1;
  };

  static const int MAX_SOURCES = 9;   // one per place: 3 ports + 6 PaHub channels
  Source source[MAX_SOURCES];

  // Tuning
  float baseSpeed   = 0.3f;           // forward cruising speed with no field (0..1)
  float forwardGain = 1.0f;
  float turnGain    = 1.0f;

  void  compute();                    // read every source and add up the vectors
  float vx() const { return _vx; }    // net vector, right
  float vy() const { return _vy; }    // net vector, forward
  void  toWheels(float& left, float& right) const;

  static const char* policyName(Policy p);

private:
  float _vx = 0;
  float _vy = 0;
};
