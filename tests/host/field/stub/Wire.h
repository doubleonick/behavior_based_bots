#pragma once
#include <Arduino.h>
// Fake devices on the Grove ports: type 'C' (TCS3472) or 'D' (VL53L0X)
struct FakeSensor { int sda, scl; bool present; char type; long value; };
extern FakeSensor fakeSensors[3]; extern int servoAngles[4];
struct TwoWire { int sda=-1, scl=-1, addr=0, nw=0, reg=0, nread=0, nbuf=0; uint8_t buf[8];
  bool begin(int a,int b,uint32_t f=0){ sda=a; scl=b; return true; } void end(){ sda=scl=-1; }
  void beginTransmission(int a){ addr=a; nw=0; } void write(int v){ if(nw==0) reg=v; else if(addr==0x38 && nw==1 && reg<4) servoAngles[reg]=v; nw++; }
  FakeSensor* here(){ for(auto&f:fakeSensors){ if(f.present && f.sda==sda && f.scl==scl) return &f; } return nullptr; }
  bool present(){ if(addr==0x38) return this==&wireMain(); if(addr==0x29) return this!=&wireMain() && here(); return false; }
  int endTransmission(bool=true){ return present()?0:2; }
  int requestFrom(int a,int n){ addr=a; if(!present()) return 0; FakeSensor* f=here(); memset(buf,0,8); nread=0;
    if(f->type=='C'){ if(reg==0x92){ buf[0]=0x44; } else if(reg==0xB4){ buf[0]=f->value&255; buf[1]=(f->value>>8)&255; } else { buf[0]=0x03; } }
    else { if(reg==0xC0){ buf[0]=0xEE; } else if(reg==0xC1){ buf[0]=0xAA; } else { buf[0]=0x17; } }
    return n; }
  int read(){ return buf[nread++]; } int available(){ return 8-nread; }
  static TwoWire& wireMain(); };
extern TwoWire Wire, Wire1;
