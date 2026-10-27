// Host test for Val_Brooks with two Key Units and cruise at the bottom, hierarchy TEST_FULL:
//   escape front collision -> avoid object -> approach light -> cruise straight.
// Story: a PaHub in Port B with the right ToF on channel 0 and the left ToF on 1 (no light
// sensors); the left Key in Port A (button G32), the right Key in Port C (button G22).
//   until 4 s   nothing around: cruise straight
//   4.0 s       left key pressed 0.1 s: back up to the left for 1 s (nose turns right)
//   6.0 s       right key pressed 0.1 s: back up to the right for 1 s
//   8.0 s       left key, then the right key 0.05 s later: both, turn on the spot for 1 s
//   10-11 s     something close on the left: avoid object turns right, 0.2 s at a time
//   11 s on     all clear: cruise again
#define ASSUME_IR_ON_PORT_B 0
#define BUMPERS 3
#define BOTTOM CRUISE_STRAIGHT
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngles[4]={-1,-1,-1,-1};
FakeHub fakeHub = {33,23,true,0};
FakeSensor fakeSensors[FAKE_SENSOR_COUNT] = {
  {-9,-9,true,'D',1500,0}, {-9,-9,true,'D',1500,1},     // distance: right on 0, left on 1
  {-9,-9,false,'C',3000,2}, {-9,-9,false,'C',3000,3} }; // no light sensors
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Brooks.ino"
int main(){
  setup();
  for(fakeMillis=5; fakeMillis<13000; fakeMillis+=5){
    float t=fakeMillis/1000.0f;
    fakeSensors[1].value = (t>=10 && t<11) ? 150 : 1500;   // left distance
    fakePinLow[32] = (t>=4.0f && t<4.1f) || (t>=8.0f && t<8.2f);    // left key (Port A)
    fakePinLow[22] = (t>=6.0f && t<6.1f) || (t>=8.05f && t<8.2f);   // right key (Port C)
    loop();
  }
}
