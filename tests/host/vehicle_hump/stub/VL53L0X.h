#pragma once
#include <Wire.h>
// Stand-in for Pololu's VL53L0X library: reads the fake distance on the bus's current pins
class VL53L0X { TwoWire* bus=nullptr; bool to=false; public:
  void setBus(TwoWire* b){ bus=b; } void setTimeout(int){} bool init(bool=true){ return bus && bus->here() && bus->here()->type=='D'; }
  void startContinuous(int=0){}
  uint16_t readRangeContinuousMillimeters(){ FakeSensor* f = bus->here(); if(!f || f->type!='D'){ to=true; return 65535; } to=false; return (uint16_t)f->value; }
  bool timeoutOccurred(){ bool t=to; to=false; return t; } };
