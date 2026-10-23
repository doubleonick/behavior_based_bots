/*
  Port - a place a sensor can be: a Grove socket's name and its two pins, and,
  for a sensor plugged into a PaHub, which of the hub's channels it is on.
  Which pin is SDA and which is SCL is worked out automatically.
*/
#pragma once

struct Port {
  const char* name;
  int pin1;
  int pin2;
  int channel;                        // PaHub channel 0..5, or NO_HUB when plugged straight in
};

const int NO_HUB = -1;

const Port PORT_A = {"Port A", 26, 32, NO_HUB};   // on the Atom Matrix
const Port PORT_B = {"Port B", 23, 33, NO_HUB};   // on the Atom Motion
const Port PORT_C = {"Port C", 19, 22, NO_HUB};   // on the Atom Motion
