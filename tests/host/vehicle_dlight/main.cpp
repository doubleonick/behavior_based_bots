// Host test for Val_Vehicle with a DLight Unit (light in lux).
// Story: a DLight in Port B (found by itself: no setting needed). Full setup: mount;
// + wire; low in a dark room (5 lux); high under a flashlight (1000 lux). Then:
// room light (300 lux: about a third of full speed), the flashlight (full speed),
// dark (stopped).
// This story has no Reflective IR or Key Unit: don't assume them in empty ports
#define ASSUME_KEY_ON_PORT_C 0
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
FakeSensor fakeSensors[3] = {{-9,-9,false,'C',0},{23,33,true,'L',5},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
int main(){
  Press presses[] = {{6.0,1.0},{7.5,1.0},{9.0,1.0},{10.5,1.0}};   // mount, +, low (dark), high (flashlight)
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<24000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    if(t<10.1){ fakeSensors[1].value=5; } else if(t<14){ fakeSensors[1].value=1000; }
    else if(t<17){ fakeSensors[1].value=300; } else if(t<20){ fakeSensors[1].value=1000; }
    else { fakeSensors[1].value=5; }
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
