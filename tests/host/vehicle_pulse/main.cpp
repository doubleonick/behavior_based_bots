// Host test for the pulse drive (a trial) on Val_Vehicle's Light Unit, + wire.
// Story: as vehicle_light (mount; Light Unit +; low covered, high lit), with
// PULSE_DRIVE on (ON_OFF beats, 1.6 a second at full). From 14 s fully lit, from
// 19 s half lit, from 31 s a quarter lit, from 54 s dark (the slower the beats, the
// longer a stretch needs to show a fair average). For each stretch the test
// draws the wheel, one character per 0.2 s (# full push, + more than a third,
// - a little, . still), and compares its average speed with the regular drive's
// (which would be the light level: 1.00, 0.50, 0.25, 0.00).
#define PORT_A_UNIT LIGHT_UNIT
#define PULSE_DRIVE 1
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
struct Stretch { float from, to; int level; const char* name; float regular; };
int main(){
  Press presses[] = {
    {6.0,1.0},{7.5,1.0},{9.0,1.0},{10.5,1.0} };          // mount, Light +, low (dark), high (lit)
  Stretch stretches[] = { {14,19,3800,"full light",1.00f}, {19,31,2000,"half light",0.50f},
                          {31,54,1100,"a quarter",0.25f}, {54,58,200,"dark",0.00f} };
  setup(); bool lastSetup=false;
  int s=-1; char strip[256]; int n=0; double sum=0; int count=0;
  for(fakeMillis=5; fakeMillis<58000; fakeMillis+=5){
    float t=fakeMillis/1000.0f; fakeButtonDown=false;
    for(auto&p:presses){ if(t>=p.at && t<p.at+p.len) fakeButtonDown=true; }
    if(t>8.5 && t<10.1){ fakeAnalog[32]=200; } else if(t>10.1 && t<11.6){ fakeAnalog[32]=3800; }
    else if(t<14){ fakeAnalog[32]=200; }
    else { for(auto&st:stretches){ if(t>=st.from && t<st.to){ fakeAnalog[32]=st.level; } } }
    loop();
    if(setupMode.active()!=lastSetup){ lastSetup=setupMode.active(); ::printf("   >> t=%.2fs setup %s\n", t, lastSetup?"STARTS":"ends"); }
    // which stretch are we in?
    int now=-1; for(int i=0;i<4;i++){ if(t>=stretches[i].from && t<stretches[i].to){ now=i; } }
    if(now!=s){
      if(s>=0){ strip[n]=0; ::printf("   >> %-10s wheel %s  average %.2f (regular drive %.2f)\n", stretches[s].name, strip, count?sum/count:0.0, stretches[s].regular); }
      s=now; n=0; sum=0; count=0;
    }
    if(s>=0 && t>=stretches[s].from+1.0f){            // after a second to settle
      float speed=(servoAngle-90)/90.0f;                // left mount: forward is toward 180
      sum+=speed; count++;
      if(fakeMillis%200==0 && n<250){ strip[n++]= speed>0.9f?'#':speed>0.33f?'+':speed>0.05f?'-':'.'; }
    }
  }
  strip[n]=0; ::printf("   >> %-10s wheel %s  average %.2f (regular drive %.2f)\n", stretches[s].name, strip, count?sum/count:0.0, stretches[s].regular);
}
