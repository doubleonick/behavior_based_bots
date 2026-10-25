#pragma once
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <algorithm>
using std::min; using std::max;
#define constrain(x,a,b) ((x)<(a)?(a):((x)>(b)?(b):(x)))
#define LOW 0
#define HIGH 1
#define INPUT 0
#define INPUT_PULLUP 2
#define INPUT_PULLDOWN 3
extern uint32_t fakeMillis; inline uint32_t millis(){ return fakeMillis; }
inline void delay(int){}
inline void delayMicroseconds(int){}
// Fake pins. A pin is either driven by a unit (fakeDriven[pin]: it reads fakeHigh[pin],
// or fakeAnalog[pin] as analog) or floating (it follows the pull-up / pull-down).
// G39 is the matrix button.
extern bool fakeButtonDown;
inline int fakeMode[40] = {};
inline bool fakeDriven[40] = {};
inline bool fakeHigh[40] = {};
inline int fakeAnalog[40] = {};
inline void pinMode(int pin, int mode){ fakeMode[pin] = mode; }
inline int digitalRead(int pin){
  if(pin==39) return fakeButtonDown ? LOW : HIGH;
  if(fakeDriven[pin]) return fakeHigh[pin] ? HIGH : LOW;
  return fakeMode[pin]==INPUT_PULLUP ? HIGH : LOW; }
inline int analogRead(int pin){ if(fakeDriven[pin]) return fakeAnalog[pin]; return 0; }
struct FakeSerial { void begin(int){} void println(const char* s){ printf("[%6.2fs] %s\n", fakeMillis/1000.0, s);} void println(){ puts(""); } void print(const char* s){ fputs(s,stdout);}
  void printf(const char* f,...){ static bool lineStart=true; if(lineStart) ::printf("[%6.2fs] ", fakeMillis/1000.0); va_list a; va_start(a,f); vprintf(f,a); va_end(a); lineStart = f[strlen(f)-1]=='\n'; } };
extern FakeSerial Serial;
