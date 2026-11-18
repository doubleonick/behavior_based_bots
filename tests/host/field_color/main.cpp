// Host test for Val_Field with one Color Unit tasting the floor: two wheels, one sensor.
// Story: a Color Unit in Port A, pointing down, mounted on the robot's LEFT side.
// Full setup: wheels; light wire OFF (two short presses), but its angle (+90: left)
// and low (sensor covered) and high (over the white floor) are still asked, because
// the colour wires share them; magenta wire PULL; yellow wire PUSH; cyan left off.
// Then the floor under the sensor changes every 3 s:
//   white floor   no colour: the robot cruises straight
//   magenta patch magenta pulls towards the sensor's side: the robot turns LEFT
//   yellow patch  yellow pushes away from the sensor's side: the robot turns RIGHT
//   cyan patch    cyan is off (and too far from both on the wheel): straight again
//   red patch     a little magenta (pull) and a little yellow (push): they cancel
// This story has no Reflective IR or Key Unit: don't assume them in empty ports
#define ASSUME_KEY_ON_PORT_C 0
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <vector>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngles[4]={-1,-1,-1,-1};
long fakeRGB[3] = {0, 0, 0};
FakeSensor fakeSensors[3] = {{32,26,true,'C',300},{-9,-9,false,'C',0},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Field.ino"
struct Press { float at, len; };
std::vector<Press> presses; float tp = 5.5f;
void shortP(){ presses.push_back({tp,0.2f}); tp += 0.6f; }
void longP(){ presses.push_back({tp,1.0f}); tp += 1.5f; }
// What the sensor sees: red, green, blue (clear = all of it)
void see(long r, long g, long b) { fakeRGB[0]=r; fakeRGB[1]=g; fakeRGB[2]=b; fakeSensors[0].value = 200 + (r + g + b) / 2; }
int main(){
  longP(); longP();                               // wheels: S1 ok, S2 ok
  shortP(); shortP(); longP();                    // light: pull -> push -> off, keep
  shortP(); shortP(); longP();                    // angle: +90 (mounted on the left)
  float lowAt=tp; longP(); float highAt=tp; longP();   // low (covered), high (white floor)
  shortP(); longP();                              // magenta: off -> pull
  shortP(); shortP(); longP();                    // yellow: off -> pull -> push
  longP();                                        // cyan: left off
  float runAt = tp + 3;
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<(uint32_t)((runAt+15)*1000); fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    if(t>=lowAt && t<lowAt+1.4f){ see(50,50,50); }                // covered
    else if(t<runAt){ see(20000,20000,20000); }                   // the white floor (and the high point)
    else if(t<runAt+3){ see(20000,20000,20000); }                 // white floor
    else if(t<runAt+6){ see(20000,1000,20000); }                  // magenta patch
    else if(t<runAt+9){ see(20000,20000,1000); }                  // yellow patch
    else if(t<runAt+12){ see(1000,20000,20000); }                 // cyan patch
    else { see(20000,1000,1000); }                                // red patch
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
    for(int k=0;k<5;k++){ if(fabsf(t-(runAt+2.5f+3*k))<0.003f){
      ::printf("   >> t=%.1fs  S1 (left) %d deg, S2 (right) %d deg\n", t, servoAngles[0], servoAngles[1]); } }
  }
}
