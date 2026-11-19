// Host test for Val_Vehicle's nerve cell screens (NEURONS_IN_SETUP 1).
// Story: Light Unit in Port A, + wire. Setup: mount; + ; low (dark), high (lit); then
// the cell: place: one short press (a cell ON the wire), keep it; threshold: the light
// is at half, long press (the cell wakes at 0.5); try it: the light goes bright (the
// cell lights up) and dim again, long press keeps it; strength: four short presses
// (the brightest shade: gain 20, all-or-none), keep it; self: keep nothing going round.
// Then the light is dim (0.3: still), bright (0.7: full speed at once), dim (still).
#define PORT_A_UNIT LIGHT_UNIT
#define NEURONS_IN_SETUP 1
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
FakeSensor fakeSensors[3] = {{-9,-9,false,'C',0},{-9,-9,false,'C',0},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
// the light as the Light Unit sees it, 0..1 (dark 200 .. lit 3800 on G32)
void lightAt(float v) { fakeAnalog[32] = (int)(200 + 3600 * v); }
int main(){
  Press presses[] = {
    {6.0,1.0},{7.5,1.0},{9.0,1.0},{10.5,1.0},             // mount, +, low, high
    {12.0,0.2},{12.5,1.0},                                // place: on the wire, keep
    {14.0,1.0},                                           // threshold: here (half)
    {17.0,1.0},                                           // try it: keep
    {18.5,0.2},{19.0,0.2},{19.5,0.2},{20.0,0.2},{20.5,1.0},   // strength: gain 20, keep
    {22.0,1.0} };                                         // self: none, keep
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<34000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    if(t>8.5 && t<10.1){ lightAt(0); } else if(t>10.1 && t<11.6){ lightAt(1); }
    else if(t>=13.0 && t<15.2){ lightAt(0.5f); }          // threshold set here
    else if(t>=15.2 && t<16.0){ lightAt(0.8f); }          // try it: bright
    else if(t>=16.0 && t<24.0){ lightAt(0.3f); }          // try it: dim
    else if(t>=24.0 && t<28.0){ lightAt(0.3f); }
    else if(t>=28.0 && t<31.0){ lightAt(0.7f); }
    else if(t>=31.0){ lightAt(0.3f); }
    else { lightAt(0.2f); }
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
