// Host test for Val_Vehicle: runs the real sketch against fake hardware (stub/).
// Story: light sensor in Port A; full setup by button presses; a distance sensor is
// plugged into Port C at 20 s (surprise, then setup for just that wire); at 44-45 s
// Port A's light sensor is swapped for a distance sensor (treated as new).
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
// Port A: light sensor. Port C: distance sensor plugged in at 20 s. Port A swapped to a distance sensor at 45 s.
FakeSensor fakeSensors[3] = {{32,26,true,'C',3000},{-9,-9,false,'C',0},{22,19,false,'D',800}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
int main(){
  Press presses[] = {
    {6.0,1.0},{7.5,1.0},{9.0,1.0},{10.5,1.0},       // full setup: mount, A +, A low, A high
    {28.5,1.0},{30.0,1.0},{31.5,1.0},              // C (distance): +, far, near
    {54.0,1.0},{55.5,1.0},{57.0,1.0} };            // A now distance: +, far, near
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<64000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    // light on A for its calibration: dark during low press, bright during high press, then middling
    if(t<44){ if(t>8.5 && t<10.1){ fakeSensors[0].value=400; } else if(t>10.1 && t<11.6){ fakeSensors[0].value=20000; } else { fakeSensors[0].value=10200; } }
    if(t>=20.0){ fakeSensors[2].present=true; }
    // distance on C: far (1000) during low press, near (100) during high press, then 550
    if(t>29.5 && t<31.1){ fakeSensors[2].value=1000; } else if(t>31.1 && t<32.6){ fakeSensors[2].value=100; } else { fakeSensors[2].value=550; }
    // at 44 s pull the light sensor out of A; at 45 s put a distance sensor in A
    if(t>=44 && t<45){ fakeSensors[0].present=false; }
    if(t>=45){ fakeSensors[0].present=true; fakeSensors[0].type='D'; if(t>55.5 && t<57.1){ fakeSensors[0].value=1000; } else if(t>57.1 && t<58.6){ fakeSensors[0].value=100; } else { fakeSensors[0].value=550; } }
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
