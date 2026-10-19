/*
  Port - a Grove socket: its name and its two pins.
  Which pin is SDA and which is SCL is worked out automatically.
*/
#pragma once

struct Port {
  const char* name;
  int pin1;
  int pin2;
};

const Port PORT_A = {"Port A", 26, 32};   // on the Atom Matrix
const Port PORT_B = {"Port B", 23, 33};   // on the Atom Motion
const Port PORT_C = {"Port C", 19, 22};   // on the Atom Motion
