// Host test for Vehicle 5's start: a neuron IN the wire that listens to itself
// (self 1, gain 4, threshold 0.5), on Val_Vehicle's Light Unit.
// Story: Light Unit in Port A, + wire. Setup as in vehicle_light. From 14 s the
// light is at 0.30 (below the threshold: still), from 17 s at 0.80 (the neuron
// switches on: full speed), from 20 s at 0.10 (the light has gone, but the neuron
// keeps itself on: still full speed), and it stays on to the end.
#define PORT_A_UNIT LIGHT_UNIT
#define PORT_A_NEURON Connection::IN_THE_WIRE
#define PORT_A_THRESHOLD 0.5f
#define NEURON_GAIN 4.0f
#define NEURON_SELF 1.0f
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
  for(fakeMillis=5; fakeMillis<29000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    // the level on G32: dark for "low" (200), lit for "high" (3800), then the story
    if(t>8.5 && t<10.1){ fakeAnalog[32]=200; } else if(t>10.1 && t<11.6){ fakeAnalog[32]=3800; }
    else if(t<14){ fakeAnalog[32]=200; }
    else if(t<17){ fakeAnalog[32]=1280; } else if(t<20){ fakeAnalog[32]=3080; }
    else { fakeAnalog[32]=560; }
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
  }
}
