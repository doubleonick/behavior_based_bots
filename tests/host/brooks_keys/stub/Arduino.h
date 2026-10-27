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
#define PI 3.14159265f
#define LOW 0
#define HIGH 1
#define INPUT 0
#define INPUT_PULLUP 2
extern uint32_t fakeMillis; inline uint32_t millis(){ return fakeMillis; }
inline void delay(int){}
inline void pinMode(int,int){}
// G39 is the Matrix button; other pins read LOW while fakePinLow[pin] is set (bump switches)
extern bool fakeButtonDown; inline bool fakePinLow[40] = {};
inline int digitalRead(int pin){ if(pin==39) return fakeButtonDown ? LOW : HIGH; return fakePinLow[pin] ? LOW : HIGH; }
// a repeatable sequence of "random" numbers
// analog pins read fakeAnalog[pin] (0..4095)
inline int fakeAnalog[40] = {};
inline int analogRead(int pin){ return fakeAnalog[pin]; }
inline uint32_t esp_random(){ static uint32_t x=12345; x = x*1103515245u + 12345u; return x>>16; }
struct FakeSerial { void begin(int){} void println(const char* s){ printf("[%6.2fs] %s\n", fakeMillis/1000.0, s);} void println(){ puts(""); } void print(const char* s){ fputs(s,stdout);} 
  void printf(const char* f,...){ static bool lineStart=true; if(lineStart) ::printf("[%6.2fs] ", fakeMillis/1000.0); va_list a; va_start(a,f); vprintf(f,a); va_end(a); lineStart = f[strlen(f)-1]=='\n'; } };
extern FakeSerial Serial;
