// Host test for Val_Field with a PaHub: runs the real sketch against fake hardware (stub/).
// Story: a PaHub in Port A with two distance sensors, the right one on channel 0 and
// the left one on channel 1 (as on the real robot). Full setup (wheels; Hub 0: push at
// -45, Hub 1: push at +45; calibration); an obstacle on the left makes the robot turn
// right, and the spike lanes are ordered left (Hub 1) to right (Hub 0). Then both
// distance sensors are unplugged and two light sensors go on the same channels:
// surprise, setup for just those two (pull, same sides, calibration); a bright light
// on the left makes the robot turn left.
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <vector>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngles[4]={-1,-1,-1,-1};
// Hub in Port A (wires either way round: the firmware tries both)
FakeHub fakeHub = {32,26,true,0};
FakeSensor fakeSensors[FAKE_SENSOR_COUNT] = {
  {-9,-9,true,'D',1000,0},  {-9,-9,true,'D',1000,1},     // distance on hub channels 0 and 1
  {-9,-9,false,'C',3000,0}, {-9,-9,false,'C',3000,1} };  // light, plugged in later
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Field.ino"
struct Press { float at, len; };
std::vector<Press> presses; float tp;
void shortP(){ presses.push_back({tp,0.2f}); tp += 0.6f; }
void longP(){ presses.push_back({tp,1.0f}); tp += 1.5f; }
// one sensor's setup: policy (default), angle +45 (1 step) or -45 (7 steps), low, high
void sensorSteps(int steps, float& lowAt, float& highAt){
  longP(); for(int k=0;k<steps;k++) shortP(); longP();
  lowAt=tp; longP(); highAt=tp; longP(); }
int main(){
  float lowAt[4], highAt[4];
  tp = 5.5f;
  longP(); longP();                                // wheels: S1 ok, S2 ok
  sensorSteps(7, lowAt[0], highAt[0]);             // Hub 0: push, -45 (right)
  sensorSteps(1, lowAt[1], highAt[1]);             // Hub 1: push, +45 (left)
  float runAt = tp + 3;
  float swapAt = runAt + 6;                        // distance sensors out, light sensors in
  float lightSetupAt = -1, lightRunAt = 1e9;
  setup(); bool lastSetup=false; int setups=0;
  for(fakeMillis=5; fakeMillis<200000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    // distance: far (1000) for low, near (100) for high
    for(int s=0;s<2;s++){
      if(t>=lowAt[s] && t<lowAt[s]+1.4f){ fakeSensors[s].value=1000; }
      else if(t>=highAt[s] && t<highAt[s]+1.4f){ fakeSensors[s].value=100; } }
    if(t>=runAt && t<runAt+3){ fakeSensors[0].value=1000; fakeSensors[1].value=300; }   // obstacle on the LEFT
    if(t>=runAt+3 && t<swapAt){ fakeSensors[0].value=1000; fakeSensors[1].value=1000; } // clear
    if(t>=swapAt){ fakeSensors[0].present=false; fakeSensors[1].present=false; }
    if(t>=swapAt+1){ fakeSensors[2].present=true; fakeSensors[3].present=true; }
    // light: covered (400) for low, lit (20000) for high
    for(int s=2;s<4;s++){
      if(t>=lowAt[s] && t<lowAt[s]+1.4f){ fakeSensors[s].value=400; }
      else if(t>=highAt[s] && t<highAt[s]+1.4f){ fakeSensors[s].value=20000; }
      else if(t<lightRunAt){ fakeSensors[s].value=3000; } }
    if(t>=lightRunAt){ fakeSensors[2].value=2000; fakeSensors[3].value=15000; }          // bright light on the LEFT
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends");
      if(lastSetup){ setups++; }
      if(lastSetup && setups==2){               // the light sensors' setup: press along with it
        lightSetupAt=t; tp=t+0.8f;
        sensorSteps(7, lowAt[2], highAt[2]);     // Hub 0: pull, -45 (right)
        sensorSteps(1, lowAt[3], highAt[3]);     // Hub 1: pull, +45 (left)
      }
      if(!lastSetup && setups==2){ lightRunAt=t+1; } }
    if(fabsf(t-(runAt+2.5f))<0.003f || fabsf(t-(runAt+5.5f))<0.003f || fabsf(t-(lightRunAt+2.5f))<0.003f){
      ::printf("   >> t=%.1fs  S1 (left) %d deg, S2 (right) %d deg\n", t, servoAngles[0], servoAngles[1]); }
    if(t>lightRunAt+3){ break; }
  }
}
