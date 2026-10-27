// Host test for Val_Vehicle's reflective IR vehicle, alone (no key).
// Story: nothing answers on I2C, so Val assumes a Reflective IR in Port B. Full setup:
// mount; IR set to - (one short press from +). Val cruises at a quarter speed forwards;
// at 14-18 s the IR sees something and the wheel turns a quarter speed backwards; then
// cruises again. At 25 s, hold for setup again: IR +; at 34-38 s it sees something
// and speeds up to three quarters.
#define ASSUME_IR_ON_PORT_B 1
#define ASSUME_KEY_ON_PORT_C 0
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false; int servoAngle=-1;
// Nothing answers on I2C in any port.
FakeSensor fakeSensors[3] = {{-9,-9,false,'C',0},{33,23,false,'C',0},{-9,-9,false,'D',0}};
FakeSerial Serial; TwoWire Wire, Wire1; FakeFastLED FastLED;
TwoWire& TwoWire::wireMain(){ return Wire; }
#include "Val_Vehicle.ino"
struct Press { float at, len; };
int main(){
  Press presses[] = {
    {6.0,1.0},{7.5,0.2},{8.5,1.0},                         // mount; IR + -> -, confirm
    {25.0,2.5},{29.0,1.0},{30.5,0.2},{31.0,0.2},{31.6,1.0} };                 // hold: setup again; mount; IR - -> x -> +, confirm
  setup(); bool lastSetup=false;
  for(fakeMillis=5; fakeMillis<42000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    // IR: its on/off output (G33) goes LOW while it sees something
    fakePinLow[33] = (t>=14 && t<18) || (t>=34 && t<38);
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
