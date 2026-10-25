// Host test for Val_Field: runs the real sketch against fake hardware (stub/).
// Story: distance sensors in Ports A and C; full setup (wheels, push policy, angles
// +45 and -45, calibration); then an obstacle on the left, then clear. The printed
// servo angles show the robot turning away from the obstacle.
// This story has no Reflective IR or Key Unit: don't assume them in empty ports
#define ASSUME_IR_ON_PORT_B 0
#define ASSUME_KEY_ON_PORT_C 0
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <vector>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngles[4]={-1,-1,-1,-1};
// Port A: distance sensor (will be set to point 45 deg left). Port C: distance sensor (45 deg right).
FakeSensor fakeSensors[3] = {{32,26,true,'D',1000},{-9,-9,false,'C',0},{22,19,true,'D',1000}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Field.ino"
struct Press { float at, len; };
std::vector<Press> presses; float tp = 5.5f;
void shortP(){ presses.push_back({tp,0.2f}); tp += 0.6f; }
void longP(){ presses.push_back({tp,1.0f}); tp += 1.5f; }
float lowAt[2], highAt[2];
int main(){
  longP(); longP();                               // wheels: S1 ok, S2 ok
  longP(); shortP(); longP();                     // A: push (default), angle +45
  lowAt[0]=tp; longP(); highAt[0]=tp; longP();    // A: low (far), high (near)
  longP(); for(int k=0;k<7;k++) shortP(); longP(); // C: push, angle -45
  lowAt[1]=tp; longP(); highAt[1]=tp; longP();    // C: low, high
  float runAt = tp + 3;
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<(uint32_t)((runAt+6)*1000); fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    for(int s=0;s<2;s++){ int port = s==0?0:2;
      if(t>=lowAt[s] && t<lowAt[s]+1.4f){ fakeSensors[port].value=1000; }
      else if(t>=highAt[s] && t<highAt[s]+1.4f){ fakeSensors[port].value=100; } }
    if(t>=runAt && t<runAt+3){ fakeSensors[0].value=300; fakeSensors[2].value=1000; }      // obstacle on the LEFT
    if(t>=runAt+3){ fakeSensors[0].value=1000; fakeSensors[2].value=1000; }                // clear
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
    if(fabsf(t-(runAt+2.5f))<0.003f || fabsf(t-(runAt+5.5f))<0.003f){ ::printf("   >> t=%.1fs  S1 (left) %d deg, S2 (right) %d deg\n", t, servoAngles[0], servoAngles[1]); }
  }
}
