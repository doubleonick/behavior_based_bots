// Host test for Val_Vehicle with units that can't say what they are.
// Story: nothing answers on I2C, so Val assumes a Reflective IR in Port B and a Key in
// Port C (both switched on below). Full setup: mount; IR +; Key + (neither is
// calibrated: both are only on or off). The key drives forwards while not pressed.
// At 14-18 s the IR sees something (adds +1). At 20-22 s the key is pressed (Val learns
// its button is on G22, the LED on G19) and the wheel goes backwards.
// At 30 s a light sensor is plugged into Port B: the IR gives way, Val is surprised and
// sets up just the light sensor.
#define ASSUME_IR_ON_PORT_B 1
#define ASSUME_KEY_ON_PORT_C 1
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
// Port A: nothing. Port B: a light sensor, plugged in at 30 s. Port C: nothing on I2C (the key).
FakeSensor fakeSensors[3] = {{-9,-9,false,'C',0},{33,23,false,'C',3000},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
int main(){
  Press presses[] = {
    {6.0,1.0},{7.5,1.0},{9.0,1.0},                         // mount, IR +, key +
    {40.0,1.0},{41.5,1.0},{43.0,1.0} };                   // light on B: +, low, high
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<50000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    // IR: its on/off output (G33) goes LOW while it sees something
    fakePinLow[33] = (t>=14 && t<18);
    // the key: pressed at 20-22 s (its button pin is G22)
    fakePinLow[22] = (t>=20 && t<22);
    // the light sensor in Port B from 30 s: covered for low, lit for high, then middling
    if(t>=30){ fakeSensors[1].present=true; }
    if(t>41.0 && t<42.6){ fakeSensors[1].value=400; } else if(t>42.6 && t<44.1){ fakeSensors[1].value=20000; } else { fakeSensors[1].value=10200; }
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
