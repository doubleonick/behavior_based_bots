// Host test for Val_Vehicle with units that can't say what they are.
// Story: the settings say a Reflective IR is in Port A and a Key in Port C, and nothing
// answers on I2C there. Full setup: mount; IR + (low with little reflected, high with a
// lot); Key + (no calibration: it is only pressed or not). The key drives forwards
// while not pressed; from 16 s the IR sees something halfway and adds to it. At
// 20-22 s the key is pressed (Val learns its button is on G22, the LED on G19) and the
// key now pulls backwards.
// At 30 s a Color Unit (light) is plugged into Port A: the IR gives way, Val is
// surprised and sets up just the light sensor.
#define PORT_A_UNIT IR_UNIT
#define ASSUME_KEY_ON_PORT_C 1
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
// Port A: a light sensor, plugged in at 30 s. Port B: nothing. Port C: nothing on I2C (the key).
FakeSensor fakeSensors[3] = {{26,32,false,'C',3000},{-9,-9,false,'C',0},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
int main(){
  Press presses[] = {
    {6.0,1.0},{7.5,1.0},{9.0,1.0},{10.5,1.0},{12.0,1.0},   // mount, IR +, IR low, IR high, key +
    {40.0,1.0},{41.5,1.0},{43.0,1.0} };                   // light on A: +, low, high
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<50000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    // IR level on G32: little reflected for "low", a lot for "high", then nothing, then halfway
    if(t>8.5 && t<10.1){ fakeAnalog[32]=300; } else if(t>10.1 && t<11.6){ fakeAnalog[32]=3500; }
    else if(t<16){ fakeAnalog[32]=300; } else { fakeAnalog[32]=1900; }
    // the key: pressed at 20-22 s (its button pin is G22)
    fakePinLow[22] = (t>=20 && t<22);
    // the light sensor in Port A from 30 s: covered for low, lit for high, then middling
    if(t>=30){ fakeSensors[0].present=true; }
    if(t>41.0 && t<42.6){ fakeSensors[0].value=400; } else if(t>42.6 && t<44.1){ fakeSensors[0].value=20000; } else { fakeSensors[0].value=10200; }
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
