// Host test for Val_Vehicle's Light Unit in Port A, alone (no key).
// Story: the settings say a Light Unit is in Port A, and nothing answers on I2C. Full
// setup: mount; Light Unit + ; low covered (dark), high lit. A plain + wire: more
// light, faster. From 14 s it is half lit (half speed), from 18 s fully lit (full
// speed), from 22 s dark again (the wheel stops).
#define PORT_A_UNIT LIGHT_UNIT
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
// Nothing answers on I2C in any port.
FakeSensor fakeSensors[3] = {{-9,-9,false,'C',0},{-9,-9,false,'C',0},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
int main(){
  Press presses[] = {
    {6.0,1.0},{7.5,1.0},{9.0,1.0},{10.5,1.0} };          // mount, Light +, low (dark), high (lit)
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<26000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    // the level on G32: dark for "low", lit for "high", then dark, half, lit, dark
    if(t>8.5 && t<10.1){ fakeAnalog[32]=200; } else if(t>10.1 && t<11.6){ fakeAnalog[32]=3800; }
    else if(t<14){ fakeAnalog[32]=200; } else if(t<18){ fakeAnalog[32]=2000; }
    else if(t<22){ fakeAnalog[32]=3800; } else { fakeAnalog[32]=200; }
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
