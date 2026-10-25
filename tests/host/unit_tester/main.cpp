// Host test for Unit_Tester: runs the real sketch against fake pins (stub/).
// Story: a Reflective IR in Port B (G23 its digital output, driven HIGH; G33 its analog
// output, 1900) and a Key in Port C (G22 its button: floating until pressed, then
// driven LOW; G19 its LED's data line: floating). Port A empty.
//   1 s: short press, show Port B.  2 s: short press, show Port C.
//   3-4 s: long press, arm the key LED test.  5.0-5.5 s: the key is pressed.
#include <Arduino.h>
#include <FastLED.h>
uint32_t fakeMillis=1; bool fakeButtonDown=false;
FakeSerial Serial; FakeFastLED FastLED;
#include "Unit_Tester.ino"
int main(){
  fakeDriven[23]=true; fakeHigh[23]=true;
  fakeDriven[33]=true; fakeAnalog[33]=1900;
  setup();
  for(fakeMillis=5; fakeMillis<7000; fakeMillis+=5){
    float t=fakeMillis/1000.0f;
    fakeButtonDown = (t>=1.0f && t<1.2f) || (t>=2.0f && t<2.2f) || (t>=3.0f && t<4.0f);
    bool keyDown = (t>=5.0f && t<5.5f);
    fakeDriven[22]=keyDown; fakeHigh[22]=false;
    loop();
  }
}
