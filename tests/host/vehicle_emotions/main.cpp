// Host test for Val_Vehicle's run endings: runs the real sketch against fake hardware (stub/).
// Story: one light sensor in Port A, wired + (full setup by button presses).
//   A bright light: it charges at full speed -> Angry for 3 s, then spikes again.
//   The light goes out: the wheel stops -> Scared.
// Then hold the button for setup again, and wire it - instead.
//   Medium light; brighter (it slows); dimmer again (it speeds up, moving on) -> Curious.
//   Very bright: the wheel stops -> Love.
// This story has no Reflective IR or Key Unit: don't assume them in empty ports
#define ASSUME_KEY_ON_PORT_C 0
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
FakeSensor fakeSensors[3] = {{32,26,true,'C',3000},{-9,-9,false,'C',0},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
// light level 0..1 as a raw reading, with calibration low 400 and high 20000
long light(float v){ return (long)(400 + v * 19600); }
int main(){
  Press presses[] = {
    {6.0,1.0},{7.5,1.0},{9.0,1.0},{10.5,1.0},       // full setup: mount, A +, A low, A high
    {30.0,2.6},                                     // hold: setup again
    {35.0,1.0},{36.5,0.2},{37.2,1.0},{38.7,1.0},{40.2,1.0} };  // mount, A: short = -, confirm, low, high
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<64000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    long v;
    if(t>8.5 && t<10.1){ v=400; } else if(t>10.1 && t<11.6){ v=20000; }   // first calibration
    else if(t<14){ v=light(0.4f); }
    else if(t<16){ v=light(0.95f); }        // bright: charging
    else if(t<21){ v=light(0.6f); }
    else if(t<29){ v=light(0.0f); }         // dark: stops
    else if(t>38.2 && t<39.8){ v=400; } else if(t>39.8 && t<41.3){ v=20000; }   // second calibration
    else if(t<44){ v=light(0.5f); }
    else if(t<47){ v=light(0.8f); }         // closer: slows
    else if(t<52){ v=light(0.3f); }         // it fades: speeds up, moving on
    else { v=light(1.0f); }                 // right next to it: stops
    fakeSensors[0].value=v;
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
