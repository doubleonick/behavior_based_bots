/*
  Connection - one wire from a sensor to the motor, in Braitenberg's sense.
  (It is called Connection because Arduino already uses the name "Wire" for I2C.)

  weight:  +1 = excitatory   (more stimulus -> faster)
           -1 = inhibitory   (more stimulus -> slower)
            0 = not connected
  transfer() is a straight line for now. Vehicle 4 is where curved
  (non-linear) connections come in, and they would go there.
*/
#pragma once
#include "Sensor.h"

class Connection {
public:
  Sensor* sensor = nullptr;
  float   weight = 1.0f;

  bool isActive() const {
    return sensor != nullptr && sensor->isConnected() && weight != 0;
  }

  float transfer(float x) const {
    return x;
  }

  // What this wire delivers to the motor
  float output() const {
    if (!isActive()) {
      return 0;
    }
    return weight * transfer(sensor->value());
  }
};
