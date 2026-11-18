// Host test for Val_Vehicle with a Color Unit set to feel one colour.
// Story: a Color Unit in Port A. Full setup: mount; channel: one short press (light ->
// magenta), keep it; + wire; low in the dark; high under a bright magenta light.
// Then the light changes every 3 s: magenta (full), red (a third: red is 60 degrees
// from magenta, and 90 degrees still counts), white (no colour: nothing), blue (a
// third), cyan (too far round the wheel: nothing), magenta at half brightness
// (half), and dark (nothing).
// This story has no Reflective IR or Key Unit: don't assume them in empty ports
#define ASSUME_KEY_ON_PORT_C 0
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
long fakeRGB[3] = {0, 0, 0};
FakeSensor fakeSensors[3] = {{32,26,true,'C',300},{-9,-9,false,'C',0},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
// The light: red, green, blue as the Color Unit reads them (clear = all of it)
void shine(long r, long g, long b) { fakeRGB[0]=r; fakeRGB[1]=g; fakeRGB[2]=b; fakeSensors[0].value = 200 + (r + g + b) / 2; }
int main(){
  Press presses[] = {
    {6.0,1.0},{7.3,0.2},{7.8,0.8},{8.8,0.8},{9.8,1.0},{11.3,1.0} };   // mount, channel: next (magenta), keep, +, low, high
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<35000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    if(t<11){ shine(100,100,100); }                       // dark (and the low point)
    else if(t<16){ shine(20000,0,20000); }                // bright magenta (the high point)
    else if(t<19){ shine(20000,0,0); }                    // red
    else if(t<22){ shine(20000,20000,20000); }            // white
    else if(t<25){ shine(0,0,20000); }                    // blue
    else if(t<28){ shine(0,20000,20000); }                // cyan
    else if(t<31){ shine(10050,50,10050); }               // magenta, half as bright
    else { shine(100,100,100); }                          // dark
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
