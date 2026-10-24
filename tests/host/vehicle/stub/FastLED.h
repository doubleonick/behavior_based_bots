#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
struct CRGB { uint8_t r,g,b; enum { Black = 0 }; CRGB():r(0),g(0),b(0){} CRGB(uint8_t R,uint8_t G,uint8_t B):r(R),g(G),b(B){} CRGB(int code):r(code>>16),g(code>>8),b(code){} };
enum { WS2812, GRB };
// Every show() adds the 25 LEDs to a checksum; it is printed when the test ends, so any
// change to what the matrix shows (not just to Serial) makes the test fail.
struct FakeFastLED { CRGB* leds=nullptr; int n=0; uint32_t frames=0; uint32_t sum=2166136261u;
  void show(){ if(!leds) return; frames++; for(int k=0;k<n;k++){ uint8_t v[3]={leds[k].r,leds[k].g,leds[k].b}; for(int j=0;j<3;j++){ sum=(sum^v[j])*16777619u; } } }
  template<int A,int B,int C> void addLeds(CRGB* l,int c){ leds=l; n=c; }
  void setBrightness(int){}
  void clear(bool s){ if(leds){ memset(leds,0,sizeof(CRGB)*n); } if(s) show(); }
  ~FakeFastLED(){ printf("LED frames shown: %u, checksum %08x\n", frames, sum); } };
extern FakeFastLED FastLED;
