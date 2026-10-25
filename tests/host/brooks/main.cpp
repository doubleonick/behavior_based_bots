// Host test for Val_Brooks: runs the real sketch against fake hardware (stub/), with the
// hierarchy left uncommented in the sketch (TEST_FULL):
//   escape front collision -> avoid object -> approach light -> wander.
// Story: a PaHub in Port A with the right ToF on channel 0, the left ToF on 1, the right
// light on 2 and the left light on 3; bump switches on Port C.
//   until 6 s   nothing around: wander rolls its die
//   6-10 s      brighter on the left: approach light (turns left)
//   10-13 s     something close on the right: avoid object (turns left), above approach light
//   13.0 s      the left bumper is pressed for 0.1 s: escape (back up, then nose turns right)
//   15 s on     all clear: wander again
// The Matrix button is held 1 s at 7 s (long press: display off) and at 16 s (on again).
// This story has no Reflective IR or Key Unit: don't assume them in empty ports
#define ASSUME_IR_ON_PORT_B 0
#define ASSUME_KEY_ON_PORT_C 0
#define BUMPERS 2   // two bump switches on Port C
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngles[4]={-1,-1,-1,-1};
FakeHub fakeHub = {32,26,true,0};
FakeSensor fakeSensors[FAKE_SENSOR_COUNT] = {
  {-9,-9,true,'D',1500,0}, {-9,-9,true,'D',1500,1},     // distance: right on 0, left on 1
  {-9,-9,true,'C',3000,2}, {-9,-9,true,'C',3000,3} };   // light: right on 2, left on 3
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Brooks.ino"
int main(){
  setup();
  for(fakeMillis=5; fakeMillis<18000; fakeMillis+=5){
    float t=fakeMillis/1000.0f;
    fakeSensors[3].value = (t>=6 && t<15) ? 9000 : 3000;   // left light
    fakeSensors[0].value = (t>=10 && t<15) ? 150 : 1500;   // right distance
    fakePinLow[LEFT_BUMPER_PIN] = (t>=13.0f && t<13.1f);
    fakeButtonDown = (t>=7.0f && t<8.0f) || (t>=16.0f && t<17.0f);
    loop();
  }
}
