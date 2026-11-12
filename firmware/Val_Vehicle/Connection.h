/*
  Connection - one wire from a sensor to the motor, in Braitenberg's sense.
  (It is called Connection because Arduino already uses the name "Wire" for I2C.)

  weight:  +1 = excitatory   (more stimulus -> faster)
           -1 = inhibitory   (more stimulus -> slower)
            0 = not connected

  Vehicles 1 to 3: the sensor is wired straight to the motor.

        sensor -----------------------> motor

  Vehicle 4 puts one nerve cell (a Neuron, see Neuron.h) on the way. Where it sits
  decides what the vehicle does:

    IN_THE_WIRE (Vehicle 4b): the signal has to go through the neuron. Nothing
    reaches the motor until the sensor feels more than the neuron's threshold.

        sensor -------> (neuron) -----> motor

    AGAINST_THE_WIRE (Vehicle 4a): the wire still goes straight to the motor, and a
    side branch goes to a neuron that works against it. Below the neuron's threshold
    only the wire acts: more stimulus, more signal. Above it the neuron holds back
    more than the wire adds: more stimulus, LESS signal. So the wire has a favourite
    level, the neuron's threshold.

        sensor -----------------------> motor
            \                            ^
             '--------> (neuron) --------'   takes away `against` x its firing

  Either way, what the wire delivers is then multiplied by the weight as before.
*/
#pragma once
#include "Sensor.h"
#include "Neuron.h"

class Connection {
public:
  enum NeuronPlace : uint8_t { NO_NEURON, IN_THE_WIRE, AGAINST_THE_WIRE };

  Sensor* sensor = nullptr;
  float   weight = 1.0f;

  NeuronPlace place = NO_NEURON;      // where this wire's neuron sits, if it has one
  Neuron  neuron;
  float   against = 2.0f;             // AGAINST_THE_WIRE: how strongly the neuron holds back

  bool isActive() const {
    return sensor != nullptr && sensor->isConnected() && weight != 0;
  }

  // What this wire delivers to the motor
  float output() {
    if (!isActive()) {
      return 0;
    }
    float felt = sensor->value();      // what the sensor feels, 0..1

    if (place == IN_THE_WIRE) {
      _passed = neuron.update(felt);
    } else if (place == AGAINST_THE_WIRE) {
      _passed = felt - against * neuron.update(felt);
      if (_passed < 0) {
        _passed = 0;                   // held back completely: nothing gets through
      }
    } else {
      _passed = felt;                  // no neuron: straight through
    }
    return weight * _passed;
  }

  // What got through the last time output() ran (for printing)
  float passed() const {
    return _passed;
  }

  static const char* placeName(NeuronPlace p) {
    if (p == IN_THE_WIRE) {
      return "neuron in the wire";
    }
    if (p == AGAINST_THE_WIRE) {
      return "neuron against the wire";
    }
    return "no neuron";
  }

private:
  float _passed = 0;
};
