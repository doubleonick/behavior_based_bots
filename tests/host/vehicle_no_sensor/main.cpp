// Host test for Val_Vehicle when setup finds no sensor at all.
// Story: a Light Unit is plugged into Port A, but the settings don't say so
// (PORT_A_UNIT is NO_UNIT, the default), and nothing answers on I2C. Full setup:
// the mount (long press), then "no sensor found" (a grey crossed box for 2 s, and a
// hint on Serial), then the check mark.
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
FakeSensor fakeSensors[3] = {{-9,-9,false,'C',0},{-9,-9,false,'C',0},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
int main(){
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<12000; fakeMillis+=5){
    float t=fakeMillis/1000.0f;
    fakeButtonDown = (t>=6.0f && t<7.0f);                 // mount: keep it
    fakeAnalog[32]=2000;                                  // the Light Unit's level, unused
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
