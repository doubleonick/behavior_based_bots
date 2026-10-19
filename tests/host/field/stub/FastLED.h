#pragma once
#include <stdint.h>
struct CRGB { uint8_t r,g,b; enum { Black = 0 }; CRGB():r(0),g(0),b(0){} CRGB(uint8_t R,uint8_t G,uint8_t B):r(R),g(G),b(B){} CRGB(int code):r(code>>16),g(code>>8),b(code){} };
enum { WS2812, GRB };
struct FakeFastLED { void show(){} template<int A,int B,int C> void addLeds(CRGB*,int){} void setBrightness(int){} void clear(bool){} };
extern FakeFastLED FastLED;
